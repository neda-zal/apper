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

#ifndef APPER_KCM_U
#define APPER_KCM_U

#include "ui_ApperKCM.h"

#include <QtGui/QStandardItemModel>

#include <KCModule>
#include <KCModuleProxy>
#include <KIcon>
#include <KToolBarPopupAction>
#include <KCategorizedSortFilterProxyModel>

#include <QPackageKit>

using namespace PackageKit;

class KpkPackageModel;
class KpkFiltersMenu;
class TransactionHistory;
class CategoryModel;
class ApperKCM : public KCModule, Ui::ApperKCM
{
    Q_OBJECT
public:
    ApperKCM(QWidget *parent, const QVariantList &args);
    ~ApperKCM();

signals:
    void changed(bool state);

public slots:
    void load();
    void save();

private slots:
    void setupHomeModel();
    void genericActionKTriggered();

    void on_backTB_clicked();
    void on_changesPB_clicked();

    void on_actionFindName_triggered();
    void on_actionFindDescription_triggered();
    void on_actionFindFile_triggered();

    void on_homeView_clicked(const QModelIndex &index);

    void finished();
    void errorCode(PackageKit::Enum::Error error, const QString &detail);

    void checkChanged();
    void changed();

private:
    void setCurrentActionEnabled(bool state);
    void setCurrentAction(QAction *action);
    void setCurrentActionCancel(bool cancel);

    void setActionCancel(bool enabled);
    void search();
    void keyPressEvent(QKeyEvent *event);

    KToolBarPopupAction *m_genericActionK;
    QAction             *m_currentAction;
    CategoryModel       *m_groupsModel;
    KCategorizedSortFilterProxyModel *m_groupsProxyModel;
    KpkPackageModel     *m_browseModel;
    KpkPackageModel     *m_changesModel;
    KCModuleProxy       *m_updatesProxy;
    KCModuleProxy       *m_settingsProxy;

    Transaction *m_searchTransaction;

    KIcon m_findIcon;
    KIcon m_cancelIcon;

    KpkFiltersMenu *m_filtersMenu;
    Enum::Roles m_roles;

    TransactionHistory *m_history;

    // Old search cache
    Enum::Role    m_searchRole;
    QString       m_searchString;
    QString       m_searchGroupCategory;
    Enum::Group   m_searchGroup;
    QModelIndex   m_searchParentCategory;
    QStringList   m_searchCategory;
    Enum::Filters m_searchFilters;
};

#endif