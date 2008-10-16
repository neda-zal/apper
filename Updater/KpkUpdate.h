/***************************************************************************
 *   Copyright (C) 2008 by Daniel Nicoletti                                *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PKUPDATE_H
#define PKUPDATE_H

#include <QPackageKit>

#include "KpkUpdateModel.h"
#include "../Common/KpkTransaction.h"
#include "../Common/KpkDelegate.h"

#include "ui_KpkUpdate.h"

using namespace PackageKit;
 
class KpkUpdate : public QWidget, Ui::KpkUpdate
{
Q_OBJECT
public:
    KpkUpdate( QWidget *parent=0 );

private slots:
    void on_updatePB_clicked();
    void on_refreshPB_clicked();
    void on_historyPB_clicked();

    void updateFinished(KpkTransaction::ExitStatus status);
    void refreshCacheFinished(KpkTransaction::ExitStatus status);

    void updateColumnsWidth(bool force = false);
    void on_packageView_pressed(const QModelIndex &index);
    void updateDetail(PackageKit::Client::UpdateInfo info);

private:
    KpkUpdateModel *m_pkg_model_updates;
    KpkDelegate *pkg_delegate;
    Client *m_client;
    Transaction *m_updatesT;
    Client::Actions m_actions;

protected:
    virtual void resizeEvent ( QResizeEvent * event );
    virtual bool event ( QEvent * event );
};

#endif
