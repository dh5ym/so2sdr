/*! Copyright 2010-2019 R. Torsten Clay N4OGW

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
#ifndef CONTEST_DXPED_H
#define CONTEST_DXPED_H

#include "cty.h"
#include "contest.h"

class Dxped : public Contest {
public:
    Dxped(QSettings &cs,QSettings &ss);
    ~Dxped();
    void setupContest(QByteArray MultFile[MMAX], const Cty * cty);
    bool validateExchange(Qso *qso);
    ContestType contestType() const { return Dxped_t;}
    void addQso(Qso *qso);
    int fieldWidth(int col) const;
    int numberField() const;
    unsigned int rcvFieldShown() const;
    unsigned int sntFieldShown() const;
    int Score() const;
    QByteArray prefillExchange(Qso *qso);
    bool showQsoPtsField() const { return false;}
    int rstField() const { return 0;}
};

#endif
