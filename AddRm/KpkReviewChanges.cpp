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

#include <KMessageBox>
#include <KDebug>

#include "KpkRequirements.h"
#include "../Common/KpkStrings.h"
#include "KpkReviewChanges.h"

#define UNIVERSAL_PADDING 6

KpkReviewChanges::KpkReviewChanges( const QList<Package*> &packages, QWidget *parent )
 : KDialog(parent), m_waitPD(0)
{
    setupUi( mainWidget() );

    //initialize the model, delegate, client and  connect it's signals
    packageView->setItemDelegate(m_pkgDelegate = new KpkDelegate(this));
    packageView->setModel(m_pkgModelMain = new KpkAddRmModel(packages, this));
    connect( m_pkgModelMain, SIGNAL( changed(bool) ), this, SLOT( enableButtonApply(bool) ) );

    setCaption( "Review Changes - KPackageKit" );

    // Set Apply and Cancel buttons
    setButtons( KDialog::Apply | KDialog::Cancel );

    label->setText( i18n("The folowing packages will be INSTALLED/REMOVED, Apply to continue:") );
}

KpkReviewChanges::~KpkReviewChanges()
{

}

void KpkReviewChanges::doAction()
{
    m_client = Client::instance();
    m_actions = m_client->getActions();
    // check what packages are installed and marked to be removed
    for (int i = 0; i < m_pkgModelMain->packagesChanges().size(); ++i) {
        if ( m_pkgModelMain->packagesChanges().at(i)->state() == Package::Installed )
            m_remPackages << m_pkgModelMain->packagesChanges().takeAt(i);
    }

    // check what packages are avaliable and marked to be installed
    for (int i = 0; i < m_pkgModelMain->packagesChanges().size(); ++i) {
        if ( m_pkgModelMain->packagesChanges().at(i)->state() == Package::Available )
            m_addPackages << m_pkgModelMain->packagesChanges().takeAt(i);
    }

    checkTask();
}

void KpkReviewChanges::checkTask()
{
    if ( !m_remPackages.isEmpty() ) {
        qDebug() << "task rm if";
        if ( m_actions.contains(Client::ActionRemovePackages) ) {
	    if ( m_actions.contains(Client::ActionGetRequires) ) {
	        m_reqDepPackages = m_remPackages;
	        // Create the requirements transaction and it's model
                m_pkgModelReq = new KpkAddRmModel(this);
		m_transactionReq = m_client->getRequires(m_reqDepPackages, Client::FilterInstalled, true);
                connect( m_transactionReq, SIGNAL( package(PackageKit::Package *) ),
		    m_pkgModelReq, SLOT( addPackage(PackageKit::Package *) ) );
                connect( m_transactionReq, SIGNAL( finished(PackageKit::Transaction::ExitStatus, uint) ),
		    this, SLOT( reqFinished(PackageKit::Transaction::ExitStatus, uint) ) );
		connect( m_transactionReq, SIGNAL( errorCode(PackageKit::Client::ErrorType, const QString&) ),
		    this, SLOT( errorCode(PackageKit::Client::ErrorType, const QString&) ) );
		// Create a KProgressDialog to don't upset the user
		m_waitPD = new KProgressDialog(this, i18n("Wait - KPackageKit"), i18n("Checking required packages") );
		m_waitPD->progressBar()->setMinimum(0);
		m_waitPD->progressBar()->setMaximum(0);
		m_waitPD->setAutoClose(false);
		m_waitPD->setModal(true);
		m_waitPD->show();
	    }
	    else {
	       removePackages();
	    }
        }
	else
	    KMessageBox::error( this, i18n("Sorry, your backend does not support removing packages"), i18n("Erro KPackageKit") );
    }
    else if ( !m_addPackages.isEmpty() ) {
        qDebug() << "task add else";
        if ( m_actions.contains(Client::ActionInstallPackages) ) {
	    if ( m_actions.contains(Client::ActionGetDepends) ) {
	        m_reqDepPackages = m_addPackages;
		// Create the depends transaction and it's model
                m_pkgModelDep = new KpkAddRmModel(this);
		m_transactionDep = m_client->getDepends(m_reqDepPackages, Client::FilterNotInstalled, true);
                connect( m_transactionDep, SIGNAL( package(PackageKit::Package *) ),
		    m_pkgModelDep, SLOT( addPackage(PackageKit::Package *) ) );
                connect( m_transactionDep, SIGNAL( finished(PackageKit::Transaction::ExitStatus, uint) ),
		    this, SLOT( depFinished(PackageKit::Transaction::ExitStatus, uint) ) );
		connect( m_transactionDep, SIGNAL( errorCode(PackageKit::Client::ErrorType, const QString&) ),
		    this, SLOT( errorCode(PackageKit::Client::ErrorType, const QString&) ) );
		// Create a KProgressDialog to don't upset the user
		m_waitPD = new KProgressDialog(this, i18n("Wait - KPackageKit"), i18n("Checking for dependencies") );
		m_waitPD->progressBar()->setMinimum(0);
		m_waitPD->progressBar()->setMaximum(0);
		m_waitPD->setAutoClose(false);
		m_waitPD->setModal(true);
		m_waitPD->show();
	    }
	    else {
	        installPackages();
	    }
        }
	else
	    KMessageBox::error( this, i18n("Sorry, your backend does not support installing packages"), i18n("Erro KPackageKit") );
    }
    else {
        qDebug() << "task else";
        KDialog::slotButtonClicked(KDialog::Ok);
    }
}

void KpkReviewChanges::reqFinished(PackageKit::Transaction::ExitStatus status, uint /*runtime*/)
{
    qDebug() << "reqFinished";
    if (status == Transaction::Success) {
	delete m_waitPD;
	m_waitPD = 0;
	if ( m_pkgModelReq->rowCount( QModelIndex() ) > 0 ) {
	    KpkRequirements *requimentD = new KpkRequirements( i18n("The following packages will also be removed for depenencies"), m_pkgModelReq, this );
	    connect( requimentD, SIGNAL( okClicked() ), this, SLOT( removePackages() ) );
	    connect( requimentD, SIGNAL( cancelClicked() ), this, SLOT( close() ) );
	    requimentD->show();
	}
	else
	    removePackages();
    }
    else {
	delete m_waitPD;
	m_waitPD = 0;
	// TODO inform the user
        qDebug() << "getReq Failed: " << status;
	m_reqDepPackages.clear();
	m_remPackages.clear();
        checkTask();
    }
    qDebug() << "reqFinished2";
}

void KpkReviewChanges::removePackages()
{
    qDebug() << "removePackages";
    if ( Transaction *t = m_client->removePackages(m_remPackages) ) {
        KpkTransaction *frm = new KpkTransaction(t, this);
        connect( frm, SIGNAL( kTransactionFinished(KpkTransaction::ExitStatus) ), this, SLOT( remFinished(KpkTransaction::ExitStatus) ) );
        frm->show();
    }
    else
        KMessageBox::error( this, i18n("Authentication failed"), i18n("KPackageKit") );
    qDebug() << "finished remove";
}

void KpkReviewChanges::depFinished(PackageKit::Transaction::ExitStatus status, uint /*runtime*/)
{
    qDebug() << "depFinished";
    if (status == Transaction::Success) {
	delete m_waitPD;
	m_waitPD = 0;
	if ( m_pkgModelDep->rowCount( QModelIndex() ) > 0 ) {
	    KpkRequirements *requimentD = new KpkRequirements( i18n("The following packages will also be installed as dependencies"), m_pkgModelDep, this );
	    connect( requimentD, SIGNAL( okClicked() ), this, SLOT( installPackages() ) );
	    connect( requimentD, SIGNAL( cancelClicked() ), this, SLOT( close() ) );
	    requimentD->show();
	}
	else
	    installPackages();
    }
    else {
	delete m_waitPD;
	m_waitPD = 0;
        qDebug() << "getDep Failed: " << status;
	m_reqDepPackages.clear();
	m_addPackages.clear();
        checkTask();
    }
    qDebug() << "depFinished2";
}

void KpkReviewChanges::installPackages()
{
    qDebug() << "installPackages";
    if ( Transaction *t = m_client->installPackages(m_addPackages) ) {
        KpkTransaction *frm = new KpkTransaction(t, this);
        connect( frm, SIGNAL( kTransactionFinished(KpkTransaction::ExitStatus) ), this, SLOT( addFinished(KpkTransaction::ExitStatus) ) );
        frm->show();
    }
    else
        KMessageBox::error( this, i18n("Authentication failed"), i18n("KPackageKit") );
    qDebug() << "finished install";
}

void KpkReviewChanges::remFinished(KpkTransaction::ExitStatus status)
{
    switch (status) {
	case KpkTransaction::Success :
	    m_remPackages.clear();
	    checkTask();
	    break;
	case KpkTransaction::Failed :
	    KMessageBox::error( this, i18n("Sorry an error occureed"), i18n("Erro KPackageKit") );
	    setButtons( KDialog::Close );
	    break;
	case KpkTransaction::Cancelled :
	    KDialog::slotButtonClicked(KDialog::Close);
	    break;
	case KpkTransaction::ReQueue :
	    KpkTransaction *trans = (KpkTransaction *) sender();
	    trans->setTransaction( m_client->removePackages(m_remPackages) );
	    break;
    }
}

void KpkReviewChanges::addFinished(KpkTransaction::ExitStatus status)
{
    switch (status) {
	case KpkTransaction::Success :
	    m_addPackages.clear();
	    checkTask();
	    break;
	case KpkTransaction::Failed :
	    KMessageBox::error( this, i18n("Sorry an error occureed"), i18n("Erro KPackageKit") );
	    setButtons( KDialog::Close );
	    break;
	case KpkTransaction::Cancelled :
	    KDialog::slotButtonClicked(KDialog::Close);
	    break;
	case KpkTransaction::ReQueue :
	    KpkTransaction *trans = (KpkTransaction *) sender();
	    trans->setTransaction( m_client->installPackages(m_addPackages) );
	    break;
    }
}

void KpkReviewChanges::slotButtonClicked(int button)
{
    switch(button) {
        case KDialog::Cancel :
            close();
            break;
        case KDialog::Apply :
            doAction();
            break;
        default :
            KDialog::slotButtonClicked(button);
    }
}

void KpkReviewChanges::errorCode(PackageKit::Client::ErrorType error, const QString &details)
{
    KMessageBox::detailedSorry( this, KpkStrings::errorMessage(error), details, KpkStrings::error(error), KMessageBox::Notify );
}

void KpkReviewChanges::resizeEvent ( QResizeEvent * event )
{
    QWidget::resizeEvent(event);
    updateColumnsWidth();
}

bool KpkReviewChanges::event ( QEvent * event )
{
    switch (event->type()) {
        case QEvent::PolishRequest:
        case QEvent::Polish:
            updateColumnsWidth(true);
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

void KpkReviewChanges::updateColumnsWidth(bool force)
{
    m_viewWidth = packageView->viewport()->width();

    if (force) {
        m_viewWidth -= style()->pixelMetric(QStyle::PM_ScrollBarExtent) + UNIVERSAL_PADDING;
    }

    packageView->setColumnWidth(0, m_pkgDelegate->columnWidth(0, m_viewWidth));
    packageView->setColumnWidth(1, m_pkgDelegate->columnWidth(1, m_viewWidth));
}

#include "KpkReviewChanges.moc"
