/***************************************************************************
 *   Copyright (C) 2008-2010 by Daniel Nicoletti                           *
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

#ifndef ADD_RM_KCM_U
#define ADD_RM_KCM_U

#include "ui_AddRmKCM.h"

#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include <KCModule>
#include <KIcon>
#include <KToolBarPopupAction>

#include <QPackageKit>

using namespace PackageKit;

class KpkPackageModel;
class KpkFiltersMenu;
class BrowseView;
class AddRmKCM : public KCModule, Ui::AddRmKCM
{
    Q_OBJECT
public:
    AddRmKCM(QWidget *parent, const QVariantList &args);
    ~AddRmKCM();

signals:
    void changed(bool state);

public slots:
    void load();
    void save();

private slots:
    void genericActionKTriggered();

    void on_backTB_clicked();

    void on_actionFindName_triggered();
    void on_actionFindDescription_triggered();
    void on_actionFindFile_triggered();

    void on_homeView_activated(const QModelIndex &index);
    void rowsAboutToBeRemoved(const QModelIndex &index, int start, int end);

    void finished(PackageKit::Enum::Exit status, uint runtime);
    void errorCode(PackageKit::Enum::Error error, const QString &detail);

    void checkChanged();
    void changed();

private:
    void setupView(KpkPackageModel **model, QTreeView *view);
    void setCurrentActionEnabled(bool state);
    void setCurrentAction(QAction *action);
    void setCurrentActionCancel(bool cancel);

    void setActionCancel(bool enabled);
    void search();
    void connectTransaction(Transaction *transaction, KpkPackageModel *model);
    void keyPressEvent(QKeyEvent *event);

    QStackedLayout *m_viewLayout;
    KToolBarPopupAction *m_genericActionK;
    QAction *m_currentAction;
    QStandardItemModel *m_groupsModel;
    BrowseView         *m_browseView;
    KpkPackageModel    *m_browseModel;
    KpkPackageModel    *m_changesModel;

    Client *m_client;
    Transaction *m_searchTransaction;

    KIcon m_findIcon;
    KIcon m_cancelIcon;

    KpkFiltersMenu *m_filtersMenu;
    Enum::Roles m_roles;

    // Old search cache
    Enum::Role    m_searchRole;
    QString       m_searchString;
    Enum::Group   m_searchGroup;
    QModelIndex   m_searchParentCategory;
    QStringList   m_searchCategory;
    Enum::Filters m_searchFilters;
};

#endif
