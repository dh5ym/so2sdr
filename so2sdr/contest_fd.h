/*! Copyright 2010-2016 R. Torsten Clay N4OGW

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
#ifndef CONTEST_FD_H
#define CONTEST_FD_H

#include "cty.h"
#include "contest.h"

class FD : public Contest {
public:
    FD();
    ~FD();
    void setupContest(QByteArray MultFile[MMAX], const Cty * cty);
    bool validateExchange(Qso *qso);
    QString cabrilloName() const
    {
        return("ARRL-FD");
    }
    QVariant columnName(int c) const;
    ContestType contestType() const
    {
        return Fd_t;
    }
    void addQso(Qso *qso);
    int fieldWidth(int col) const;
    int numberField() const;
    unsigned int rcvFieldShown() const;
    unsigned int sntFieldShown() const;
    int Score() const;
};

#endif
