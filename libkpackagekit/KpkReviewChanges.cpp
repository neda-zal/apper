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

#include "KpkReviewChanges.h"

#include <KMessageBox>
#include <KWindowSystem>
#include <KCategorizedSortFilterProxyModel>

#include <KDebug>

#include <KpkMacros.h>
#include "KpkStrings.h"
#include "KpkEnum.h"
#include "KpkRequirements.h"
#include "KpkSimulateModel.h"
#include "KpkPackageModel.h"
#include "KpkTransaction.h"
#include "KpkDelegate.h"

#include "ui_KpkReviewChanges.h"

class KpkReviewChangesPrivate
{
public:
    Ui::KpkReviewChanges ui;

    KpkPackageModel *mainPkgModel;
    KpkSimulateModel *installPkgModel, *removePkgModel;
    KpkDelegate *pkgDelegate;

    Client *client;

    QList<QSharedPointer<PackageKit::Package> > remPackages;
    QList<QSharedPointer<PackageKit::Package> > addPackages;
    QList<QSharedPointer<PackageKit::Package> > reqDepPackages;

    Enum::Roles actions;
    uint parentWId;

    KpkTransaction *transactionDialog;
};

KpkReviewChanges::KpkReviewChanges(const QList<QSharedPointer<PackageKit::Package> > &packages,
                                   QWidget *parent,
                                   uint parentWId)
 : KDialog(parent),
   d(new KpkReviewChangesPrivate),
   m_flags(Default)
{
    d->ui.setupUi(mainWidget());

    d->client = Client::instance();
    d->transactionDialog = 0;
    d->parentWId = parentWId;

    if (parentWId) {
        KWindowSystem::setMainWindow(this, parentWId);
    }

    //initialize the model, delegate, client and  connect it's signals
    d->ui.packageView->viewport()->setAttribute(Qt::WA_Hover);
    d->mainPkgModel = new KpkPackageModel(this);
    KCategorizedSortFilterProxyModel *changedProxy = new KCategorizedSortFilterProxyModel(this);
    changedProxy->setSourceModel(d->mainPkgModel);
    changedProxy->setCategorizedModel(true);
    changedProxy->sort(0);
    changedProxy->setDynamicSortFilter(true);
    changedProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    changedProxy->setSortRole(KpkPackageModel::SortRole);
    d->ui.packageView->setModel(changedProxy);
    d->pkgDelegate = new KpkDelegate(d->ui.packageView);
    d->pkgDelegate->setExtendPixmapWidth(0);
    d->ui.packageView->setItemDelegate(d->pkgDelegate);
    d->mainPkgModel->addPackages(packages, true);
    d->mainPkgModel->finished();
    connect(d->mainPkgModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
            this, SLOT(checkChanged()));

    // Set Apply and Cancel buttons
    setButtons(KDialog::Apply | KDialog::Cancel);
    setWindowModality(Qt::WindowModal);

    foreach (const QSharedPointer<PackageKit::Package> &p, packages) {
        if (p->info() == Enum::InfoInstalled ||
            p->info() == Enum::InfoCollectionInstalled) {
            // check what packages are installed and marked to be removed
            d->remPackages << p;
        } else if (p->info() == Enum::InfoAvailable ||
                   p->info() == Enum::InfoCollectionAvailable) {
            // check what packages are available and marked to be installed
            d->addPackages << p;
        }
    }

    setCaption(i18np("Review Change", "Review Changes", packages.size()));
    setMessage(i18np("The following package was found",
                     "The following packages were found",
                     packages.size()));

    // restore size
    setMinimumSize(QSize(320,280));
    KConfig config("KPackageKit");
    KConfigGroup reviewChangesDialog(&config, "ReviewChangesDialog");
    restoreDialogSize(reviewChangesDialog);
}

KpkReviewChanges::~KpkReviewChanges()
{
    // Make sure the dialog is deleted in case we are not it's parent
    if (d->transactionDialog) {
        d->transactionDialog->deleteLater();
    }

    // save size
    KConfig config("KPackageKit");
    KConfigGroup reviewChangesDialog(&config, "ReviewChangesDialog");
    saveDialogSize(reviewChangesDialog);
    delete d;
}

void KpkReviewChanges::setMessage(const QString &msg)
{
    d->ui.label->setText(msg);
}

int KpkReviewChanges::exec(OperationModes flags)
{
    m_flags = flags;
    if (m_flags & ShowConfirmation) {
        show();
    } else {
        // Starts the action without showing the dialog
        QTimer::singleShot(0, this, SLOT(doAction()));
    }

    QEventLoop loop;
    connect(this, SIGNAL(finished(int)), &loop, SLOT(quit()));
    loop.exec();

    return QDialog::Accepted;
}

void KpkReviewChanges::doAction()
{
    // Fix up the parent when this class is not shown
    QWidget *transParent = qobject_cast<QWidget*>(parent());
    if (m_flags & ShowConfirmation) {
        transParent = this;
    }

    d->actions = d->client->actions();

    if (!d->addPackages.isEmpty() || !d->remPackages.isEmpty()) {
        d->transactionDialog = new KpkTransaction(0,
                                                  KpkTransaction::Modal,
                                                  transParent);
        connect(d->transactionDialog, SIGNAL(finished(KpkTransaction::ExitStatus)),
                this, SLOT(transactionFinished(KpkTransaction::ExitStatus)));
        if (d->parentWId) {
            KWindowSystem::setMainWindow(d->transactionDialog, d->parentWId);
        }
        d->transactionDialog->show();

        checkTask();
    } else {
        reject();
    }
}

void KpkReviewChanges::checkTask()
{
//     kDebug() << "AddPackages" << !d->addPackages.isEmpty()
//              << "RemovePackages" << !d->remPackages.isEmpty();
    if (!d->remPackages.isEmpty()) {
        if (d->actions & Enum::RoleRemovePackages) {
            if (d->actions & Enum::RoleSimulateRemovePackages/* &&
                !(m_flags & HideConfirmDeps)*/) { //TODO we need admin to lock this down
                d->reqDepPackages = d->remPackages;
                d->removePkgModel = new KpkSimulateModel(this, d->reqDepPackages);
                // Create the requirements transaction and it's model
                Transaction *trans;
                trans = d->client->simulateRemovePackages(d->reqDepPackages, AUTOREMOVE);
                if (trans->error()) {
                    KMessageBox::sorry(this,
                                       KpkStrings::daemonError(trans->error()),
                                       i18n("Failed to simulate package removal"));
                    taskDone(Enum::RoleRemovePackages);
                } else {
                    d->transactionDialog->setTransaction(trans);
                    connect(trans, SIGNAL(package(QSharedPointer<PackageKit::Package>)),
                            d->removePkgModel, SLOT(addPackage(QSharedPointer<PackageKit::Package>)));
                }
            } else {
                // As we can't check for requires don't allow deps removal
                removePackages(false);
            }
        } else {
            KMessageBox::error(this, i18n("The current backend does not support removing packages."), i18n("KPackageKit Error"));
            taskDone(Enum::RoleRemovePackages);
        }
    } else if (!d->addPackages.isEmpty()) {
        if (d->actions & Enum::RoleInstallPackages) {
            if (d->actions & Enum::RoleSimulateInstallPackages &&
                !(m_flags & HideConfirmDeps)) {
                d->reqDepPackages = d->addPackages;
                d->installPkgModel = new KpkSimulateModel(this, d->reqDepPackages);
                // Create the depends transaction and it's model
                Transaction *trans;
                trans = d->client->simulateInstallPackages(d->reqDepPackages);
                if (trans->error()) {
                    KMessageBox::sorry(this,
                                       KpkStrings::daemonError(trans->error()),
                                       i18n("Failed to simulate package install"));
                    taskDone(Enum::RoleInstallPackages);
                } else {
                    d->transactionDialog->setTransaction(trans);
                    connect(trans, SIGNAL(package(QSharedPointer<PackageKit::Package>)),
                            d->installPkgModel, SLOT(addPackage(QSharedPointer<PackageKit::Package>)));
                }
            } else {
                installPackages();
            }
        } else {
            KMessageBox::error(this, i18n("Current backend does not support installing packages."), i18n("KPackageKit Error"));
            taskDone(Enum::RoleInstallPackages);
        }
    } else {
        slotButtonClicked(KDialog::Ok);
    }
}

void KpkReviewChanges::installPackages()
{
    SET_PROXY
    QString socket;
    socket = "/tmp/kpk_debconf_" + QString::number(QCoreApplication::applicationPid());
    d->client->setHints("frontend-socket=" + socket);
    Transaction *trans = d->client->installPackages(true, d->addPackages);
    if (trans->error()) {
        KMessageBox::sorry(this,
                           KpkStrings::daemonError(trans->error()),
                           i18n("Failed to install package"));
        taskDone(Enum::RoleInstallPackages);
    } else {
        d->transactionDialog->setTransaction(trans);
        d->transactionDialog->setupDebconfDialog(socket);
        d->transactionDialog->setPackages(d->addPackages);
    }
}

void KpkReviewChanges::removePackages(bool allowDeps)
{
    SET_PROXY
    QString socket;
    socket = "/tmp/kpk_debconf_" + QString::number(QCoreApplication::applicationPid());
    d->client->setHints("frontend-socket=" + socket);
    Transaction *trans = d->client->removePackages(d->remPackages, allowDeps, AUTOREMOVE);
    if (trans->error()) {
        KMessageBox::sorry(this,
                           KpkStrings::daemonError(trans->error()),
                           i18n("Failed to remove package"));
        taskDone(Enum::RoleRemovePackages);
    } else {
        d->transactionDialog->setTransaction(trans);
        d->transactionDialog->setupDebconfDialog(socket);
        d->transactionDialog->setAllowDeps(allowDeps);
        d->transactionDialog->setPackages(d->remPackages);
    }
}

void KpkReviewChanges::transactionFinished(KpkTransaction::ExitStatus status)
{
    KpkTransaction *trans = qobject_cast<KpkTransaction*>(sender());
    if (status == KpkTransaction::Success) {
        switch (trans->role()) {
        case Enum::RoleSimulateRemovePackages:
            if (d->removePkgModel->rowCount() > 0) {
                KpkRequirements *req = new KpkRequirements(d->removePkgModel,
                                                           d->transactionDialog);
                connect(req, SIGNAL(accepted()), this, SLOT(removePackages()));
                connect(req, SIGNAL(rejected()), this, SLOT(reject()));
                req->show();
            } else {
                // As there was no requires don't allow deps removal
                removePackages(false);
            }
            break;
        case Enum::RoleSimulateInstallPackages:
            if (d->installPkgModel->rowCount() > 0) {
                KpkRequirements *req = new KpkRequirements(d->installPkgModel,
                                                           d->transactionDialog);
                connect(req, SIGNAL(accepted()), this, SLOT(installPackages()));
                connect(req, SIGNAL(rejected()), this, SLOT(reject()));
                req->show();
            } else {
                installPackages();
            }
            break;
        case Enum::RoleRemovePackages:
            emit successfullyRemoved();
            taskDone(trans->role());
            break;
        case Enum::RoleInstallPackages:
            emit successfullyInstalled();
            taskDone(trans->role());
            break;
        default:
            kWarning() << "Role not Handled" << trans->role();
            break;
        }
    } else {
        slotButtonClicked(KDialog::Cancel);
    }
}

void KpkReviewChanges::taskDone(PackageKit::Enum::Role role)
{
    if (role == Enum::RoleRemovePackages) {
        d->remPackages.clear();
    } else if (role == Enum::RoleInstallPackages) {
        d->addPackages.clear();
    }
    checkTask();
}

void KpkReviewChanges::slotButtonClicked(int button)
{
    switch(button) {
    case KDialog::Cancel :
    case KDialog::Close :
        reject();
        break;
    case KDialog::Ok :
        accept();
        break;
    case KDialog::Apply :
        hide();
        doAction();
        break;
    default :
        KDialog::slotButtonClicked(button);
    }
}

void KpkReviewChanges::checkChanged()
{
    if (d->mainPkgModel->selectedPackages().size() > 0) {
        enableButtonApply(true);
    } else {
        enableButtonApply(false);
    }
}

#include "KpkReviewChanges.moc"
