/***************************************************************************
 *   Copyright (C) 2010 by Daniel Nicoletti                                *
 *   dantti85-pk@yahoo.com.br                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#ifndef TRANSACTION_TRAY_ICON_H
#define TRANSACTION_TRAY_ICON_H

#include <KStatusNotifierItem>

class QAction;

namespace PackageKit {
    class Transaction;
}

class TransactionTrayIcon : public KStatusNotifierItem
{
Q_OBJECT
public:
    TransactionTrayIcon(QObject *parent = 0);
    ~TransactionTrayIcon();

signals:
    void transactionActivated(PackageKit::Transaction *transaction);

public slots:
    void actionActivated(QAction *action);
};

#endif
