/**************************************************************************
*   Copyright (C) 2009 by Trever Fischer                                  *
*   wm161@wm161.net                                                       *
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

#include "KpkTransactionBar.h"

#include <KColorScheme>
#include <KFadeWidgetEffect>
#include <KpkStrings.h>
#include <KLocalizedString>
#include <KLocale>

#include <KDebug>


KpkTransactionBar::KpkTransactionBar(QWidget *parent)
    : QWidget(parent)
{
    m_label = new QLabel(this);
    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    m_progress = new QProgressBar(this);
    m_progress->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_cancel = new KPushButton(i18n("Cancel"), this);
    m_timer = new QTimer(this);

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_progress);
    m_layout->addWidget(m_cancel);
    m_layout->addWidget(m_label);

    setLayout(m_layout);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(hide()));

}

KpkTransactionBar::~KpkTransactionBar()
{
}

void KpkTransactionBar::setBehaviors(KpkTransactionBar::Behaviors flags)
{
    m_flags = flags;
    if (m_flags & AutoHide && m_trans.size()==0)
        hide();
    else if (m_flags & AutoHide && m_trans.size()>0)
        show();
    kDebug() << "Hide!" << m_flags;
}

KpkTransactionBar::Behaviors KpkTransactionBar::behaviors() const
{
    return m_flags;
}

void KpkTransactionBar::nextTransaction()
{
    if (m_trans.size()==0)
        return;
    m_progress->reset();
    m_progress->setMaximum(0);
    m_progress->setMinimum(0);
    if (m_flags & AutoHide)
        show();
    m_timer->stop();
    setPalette(QPalette());
    setAutoFillBackground(false);
    Transaction* trans = m_trans.takeFirst();

    enableButtonCancel( trans->allowCancel() );

    m_label->setText( KpkStrings::status( trans->status() ) );

    progressChanged(trans->progress());
    statusChanged(trans->status());

    connect( trans, SIGNAL( finished(PackageKit::Transaction::ExitStatus, uint) ),
        this, SLOT( finished(PackageKit::Transaction::ExitStatus, uint) ) );
    connect( trans, SIGNAL( allowCancelChanged(bool) ),
        this, SLOT( enableButtonCancel(bool) ) );
    connect( trans, SIGNAL( errorCode(PackageKit::Client::ErrorType, const QString&) ),
        this, SLOT( errorCode(PackageKit::Client::ErrorType, const QString&) ) );
    connect( trans, SIGNAL( progressChanged(PackageKit::Transaction::ProgressInfo) ),
        this, SLOT( progressChanged(PackageKit::Transaction::ProgressInfo) ) );
    connect( trans, SIGNAL( statusChanged(PackageKit::Transaction::Status) ),
        this, SLOT( statusChanged(PackageKit::Transaction::Status) ) );
}

void KpkTransactionBar::addTransaction(Transaction *trans)
{
    m_trans.append(trans);
    if (m_trans.size()==1)
        nextTransaction();
}

void KpkTransactionBar::finished(Transaction::ExitStatus status, uint runtime)
{
    QPalette colors(palette());
    switch (status) {
        case Transaction::Success:
            KColorScheme::adjustBackground(colors, KColorScheme::PositiveBackground, QPalette::Window, KColorScheme::Window);
            m_label->setText(i18n("Finished in %1.", KGlobal::locale()->formatDuration(runtime)));
            break;
        default:
            KColorScheme::adjustBackground(colors, KColorScheme::NegativeBackground, QPalette::Window, KColorScheme::Window);
    }
    m_progress->setValue(100);
    setAutoFillBackground(true);
    setPalette(colors);
    KFadeWidgetEffect *animation = new KFadeWidgetEffect(this);
    setAutoFillBackground(false);
    setPalette(QPalette());
    animation->start();
    if (m_flags & AutoHide)
        m_timer->start(2000);
    nextTransaction();
}

void KpkTransactionBar::errorCode(Client::ErrorType type, const QString &details)
{
    Q_UNUSED(details);
    m_label->setText( KpkStrings::error(type) );
}

void KpkTransactionBar::progressChanged(Transaction::ProgressInfo progress)
{
    m_progress->setMaximum(100);
    m_progress->setValue(progress.percentage);
}

void KpkTransactionBar::statusChanged(Transaction::Status status)
{
    m_label->setText( KpkStrings::status(status) );
}

void KpkTransactionBar::enableButtonCancel(bool cancel)
{
    m_cancel->setEnabled(cancel);
}