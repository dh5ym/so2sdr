/*! Copyright 2010-2012 rec. Torsten Clay N4OGW

   This file is part of so2sdrec.

    so2sdr is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    so2sdr is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with so2sdrec.  If not, see <http://www.gnu.org/licenses/>.

 */
#include "mytableview.h"
#include <QKeyEvent>
#include <QDebug>

MyTableView::MyTableView(QWidget *w) : QTableView(w)
{
   installEventFilter(this);
}

/*! event filter for log display delegate
 */
bool MyTableView::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type()==QEvent::KeyPress) {
        QKeyEvent* kev = static_cast<QKeyEvent*>(event);
        if (kev->key()==Qt::Key_Escape) {
            return false;
        }
        /*
        if (kev->key()==Qt::Key_E) {
            if (kev->modifiers()==Qt::ControlModifier) {
                emit(startDetailedEdit());
            }
        }*/
    }
    return QTableView::eventFilter(obj, event);
}