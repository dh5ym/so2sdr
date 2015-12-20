/*! Copyright 2010-2015 R. Torsten Clay N4OGW

   This file is part of so2sdr.

    so2sdr is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    so2sdr is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with so2sdr.  If not, see <http://www.gnu.org/licenses/>.

 */
#include <QSettings>
#include "defines.h"
#include "winkey.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

/*!
   WinkeyDevice : serial port of device
 */
Winkey::Winkey(QSettings &s, QObject *parent) : QObject(parent), settings(s)
{
    QSerialPortInfo info(settings.value(s_winkey_device,s_winkey_device_def).toString());
    winkeyPort = new QSerialPort(info);

    winkeyVersion  = 0;
    nchar          = 0;
    sendBuff       = "";
    sending        = false;
    winkeyOpen     = false;
    winkeySpeedPot = 0;
    rigNum         = 0;
    initStatus     = 0;
}

Winkey::~Winkey()
{
    closeWinkey();
    delete winkeyPort;
}

/*!
   returns true if winkey has been opened successfully
 */
bool Winkey::winkeyIsOpen() const
{
    return(winkeyOpen);
}

/*!
   returns true if winkey is sending cw
 */
bool Winkey::isSending() const
{
    return(sending);
}

/*!
   Slot triggered when data is available at port
 */
void Winkey::receive()
{
    unsigned char wkbyte;

    int           n = winkeyPort->read((char *) &wkbyte, 1);
    if (n > 0) {
        if ((wkbyte & 0xc0) == 0xc0) {
            // Status byte: currently sending CW?
            if (wkbyte & 4) {
                sending = true;
                emit(winkeyTx(true, rigNum));
            } else {
                sending = false;
                emit(winkeyTx(false, rigNum));
            }
        // Pushbutton status only sent to host in WK2 mode by admin command, (0x00, 11)
        } else if ((wkbyte & 0xc0) == 0x80) {
            // speed pot setting in 6 lowest bits
            winkeySpeedPot = wkbyte & 0x3f;
        } else    {
            // This would be an echo byte
        }
    }
}

/*!
   Slot triggered when data is available at port, used during winkey
   initialization
 */
void Winkey::receiveInit()
{
    unsigned char wkbyte;

    int n = winkeyPort->read((char *) &wkbyte, 1);
    if (n > 0) {
        if (wkbyte==0x55) {
            // this was the echo test
            initStatus=1;
        } else {
            // otherwise assume this is version number
            winkeyVersion = wkbyte;
        }
    }
}


/*!
   load a message into buffer
 */
void Winkey::loadbuff(QByteArray msg)
{
    sendBuff.append(msg);
    nchar = sendBuff.length();
}

/*!
   Slot to start sending cw
 */
void Winkey::sendcw()
{
    if (winkeyPort->isOpen()) {
        winkeyPort->write(sendBuff.data(), nchar);
    } else {
        winkeyOpen = false;
    }
    nchar = 0;
    sendBuff.resize(0);
}

/*!
   cancel winkey sending (command=10)
 */
void Winkey::cancelcw()
{
    const unsigned char cancel = 0x0a;

    if (winkeyPort->isOpen()) {
        winkeyPort->write((char *) &cancel, 1);
    } else {
        winkeyOpen = false;
    }
    sending=false;
    sendBuff.resize(0);
    nchar = 0;
}


/*!
   set speed directly in WPM
 */
void Winkey::setSpeed(int speed)
{
    if (!winkeyPort->isOpen() || speed < 5 || speed > 99) {
        return;
    }
    unsigned char buff[2];
    buff[0] = 0x02;
    buff[1] = (unsigned char) speed;
    winkeyPort->write((char *) buff, 2);
}

/*!
   set output port on Winkey
   sets PTT, sidetone on for either
 */
void Winkey::switchTransmit(int nrig)
{
    if (!winkeyPort->isOpen()) return;
    rigNum = nrig;
    unsigned char buff[2];
    buff[0] = 0x09;
    switch (nrig) {
    case 0:
        buff[1] = 0x04 + 0x01 + 0x02;
        break;
    case 1:
        buff[1] = 0x08 + 0x01 + 0x02;
        break;
    default:
        return;
    }
    winkeyPort->write((char *) buff, 2);
}

/*!
   start Winkey and get version number
 */
void Winkey::openWinkey()
{
    // in case we are re-starting winkey
    if (winkeyPort->isOpen()) {
        closeWinkey();
        delete winkeyPort;
        QSerialPortInfo info(settings.value(s_winkey_device,s_winkey_device_def).toString());
        winkeyPort = new QSerialPort(info);
    }

    winkeyPort->setBaudRate(QSerialPort::Baud1200);
    winkeyPort->setDataBits(QSerialPort::Data8);
    winkeyPort->setStopBits(QSerialPort::TwoStop);
    winkeyPort->setParity(QSerialPort::NoParity);
    winkeyPort->setFlowControl(QSerialPort::NoFlowControl);
    winkeyPort->open(QIODevice::ReadWrite);
    if (!winkeyPort->isOpen()) {
        winkeyOpen = false;
        return;
    }
    connect(winkeyPort,SIGNAL(readyRead()),this,SLOT(receiveInit()));
    winkeyPort->setRequestToSend(false);
    winkeyPort->setDataTerminalReady(true);

    // Send three null commands to resync host to WK2
    unsigned char buff[64];
    buff[0] = 0x13;
    winkeyPort->write((char *) buff, 1);
    winkeyPort->write((char *) buff, 1);
    winkeyPort->write((char *) buff, 1);

    winkeyPort->waitForBytesWritten(500);;
    winkeyPort->waitForReadyRead(500);

    // Echo Test to see if WK is really there
    buff[0] = 0x00;     // WK admin command, next byte sets admin function
    buff[1] = 4;        // Echo function, echoes next received character to host
    buff[2] = 0x55;     // Send 'U' to WK
    winkeyPort->write((char *) buff, 3);
    winkeyPort->waitForBytesWritten(500);
    winkeyPort->waitForReadyRead(500);

    // Was the 'U' received?
    if (initStatus==1) {
        buff[0] = 0x00;     // WK admin command
        buff[1] = 2;        // Host open, WK will now receive commands and Morse characters
        winkeyPort->write((char *) buff, 2);
        winkeyPort->waitForBytesWritten(500);
        winkeyPort->waitForReadyRead(500);

        if (winkeyVersion == 0) {
            // winkey open failed
            qDebug("Winkey: open failed, could not get version");
            closeWinkey();
            winkeyOpen = false;
            initStatus=0;
            disconnect(winkeyPort,SIGNAL(readyRead()),this,SLOT(receiveInit()));
            return;
        } else {
            initStatus=2;
            emit(version(winkeyVersion));
        }

        // now set some saved user settings
        // set sidetone config
        buff[0] = 0x01;     // Sidetone control command, next byte sets sidetone parameters
        buff[1] = 0;

        // Paddle sidetone only?  Set bit 7 (msb) of buff[1]
        if (settings.value(s_winkey_sidetonepaddle,s_winkey_sidetonepaddle_def).toBool()) {
            buff[1] += 128;
        }
        // Set sidetone frequency (chosen in GUI)
        buff[1] += settings.value(s_winkey_sidetone,s_winkey_sidetone_def).toInt();

        winkeyPort->write((char *) buff, 2);
        winkeyPort->waitForBytesWritten(500);

        // set other winkey features
        buff[0] = 0x0e;     // Set WK options command, next byte sets WK options
        buff[1] = 0;
        // CT spacing?  Set bit 0 (lsb) of buff[1]
        if (settings.value(s_winkey_ctspace,s_winkey_ctspace_def).toBool()) {
            buff[1] += 1;
        }
        // Paddle swap?  Set bit 3 of buff[1]
        if (settings.value(s_winkey_paddle_swap,s_winkey_paddle_swap_def).toBool()) {
            buff[1] += 8;
        }
        // Paddle mode, set bits 5,4 to bit mask, 00 = iambic B, 01 = iambic A,
        // 10 = ultimatic, 11 = bug
        buff[1] += (settings.value(s_winkey_paddle_mode,s_winkey_paddle_mode_def).toInt()) << 4;

        winkeyPort->write((char *) buff, 2);
        winkeyPort->waitForBytesWritten(500);

        // Pot min/max
        // must set this up or paddle speed screwed up.
        // winkey bug/undocumented feature?
        buff[0] = 0x05;     // Setup speed pot command, next three bytes setup the speed pot
        buff[1] = 10;       // min wpm
        buff[2] = 80;       // wpm range (min wpm + wpm range = wpm max)
        buff[3] = 0;        // Used only on WK1 keyers (does 0 cause a problem on WK1?)
        winkeyPort->write((char *) buff, 4);
        winkeyPort->waitForBytesWritten(500);

        winkeyOpen = true;
        // disconnect slot used for receive during init process
        disconnect(winkeyPort,SIGNAL(readyRead()),this,SLOT(receiveInit()));
    } else {
        qDebug("Winkey: echo test failed");
    }
}

void Winkey::closeWinkey()
{
    unsigned char buff[2];
    buff[0] = 0x00;     // Admin command, next byte is function
    buff[1] = 0x03;     // Host close
    winkeyPort->write((char *) &buff, 2);
    if (winkeyPort->isOpen()) {
        winkeyPort->flush();
    }
    winkeyPort->close();
    disconnect(winkeyPort,SIGNAL(readyRead()));
    initStatus=0;
    winkeyOpen=false;
}


