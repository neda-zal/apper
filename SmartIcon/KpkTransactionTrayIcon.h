/***************************************************************************
 *   Copyright (C) 2008-2009 by Daniel Nicoletti                           *
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

#ifndef KPK_TRANSACTION_TRAY_ICON_H
#define KPK_TRANSACTION_TRAY_ICON_H

#include <KpkAbstractIsRunning.h>

#include <QPackageKit>
#include <KSystemTrayIcon>

using namespace PackageKit;

class KpkTransaction;
class KpkTransactionTrayIcon : public KpkAbstractIsRunning
{
Q_OBJECT
public:
    KpkTransactionTrayIcon(QObject *parent = 0);
    ~KpkTransactionTrayIcon();

    bool isRunning();

signals:
    void removeTransactionWatcher(const QString &tid);

public slots:
    void checkTransactionList();

private slots:
    void transactionListChanged(const QList<PackageKit::Transaction*> &tids);
    void activated(QSystemTrayIcon::ActivationReason reason);
    void triggered(QAction *action);
    void createTransactionDialog(Transaction *t);
    void transactionDialogClosed();
    void message(PackageKit::Client::MessageType type, const QString &message);
    void requireRestart(PackageKit::Client::RestartType type, Package *pkg);
    void finished(PackageKit::Transaction::ExitStatus status, uint runtime);
    void restartActivated(uint action = 1);
    void transactionChanged();
    void logout();

    void refreshCache();
    void showMessages();

private:
    void updateMenu(const QList<PackageKit::Transaction*> &tids);
    void setCurrentTransaction(PackageKit::Transaction *transaction);

    Client::Actions m_act;
    KSystemTrayIcon *m_smartSTI;
    Client *m_client;
    Transaction *m_pkClient_updates;
    QMenu *m_menu;
    QHash<QString, KpkTransaction *> m_transDialogs;

    PackageKit::Transaction *m_currentTransaction;

    // Refresh Cache menu entry
    QAction *m_refreshCacheAction;

    // Message Container
    QList<QPair<Client::MessageType, QString> > m_messages;
    QAction *m_messagesAction;

    // Restart menu entry
    Client::RestartType m_restartType;
    QAction *m_restartAction;
    QStringList m_restartPackages;

    // Hide this icon action
    QAction *m_hideAction;
};

#endif
