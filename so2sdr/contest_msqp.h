/*! Copyright 2010-2020 R. Torsten Clay N4OGW

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
#ifndef CONTEST_MSQP_H
#define CONTEST_MSQP_H

#include "contest.h"

// Mississippi QSO party; updated for 2019 rules

class MSQP : public Contest {
public:
    MSQP(QSettings &cs,QSettings &ss);
    ~MSQP();
    ContestType contestType() const { return Msqp_t;}
    void setupContest(QByteArray MultFile[MMAX], const Cty * cty);
    bool validateExchange(Qso *qso);
    void addQso(Qso *qso);
    int fieldWidth(int col) const;
    int numberField() const;
    unsigned int rcvFieldShown() const;
    bool showQsoPtsField() const { return true;}
    unsigned int sntFieldShown() const;
    void setWithinState(bool);
private:
    bool withinState;
};

#endif
