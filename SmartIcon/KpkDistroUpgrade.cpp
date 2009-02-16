/***************************************************************************
 *   Copyright (C) 2009 by Daniel Nicoletti                                *
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

#include "KpkDistroUpgrade.h"

#include <KNotification>
#include <KLocale>
#include <KIcon>

#include <KDebug>

KpkDistroUpgrade::KpkDistroUpgrade(QObject *parent)
 : KpkAbstractSmartIcon(parent),
   m_distroUpgradeProcess(0)
{
}

KpkDistroUpgrade::~KpkDistroUpgrade()
{
}

void KpkDistroUpgrade::checkDistroUpgrades()
{
    if (!isRunning()) {
        if (Transaction *t = Client::instance()->getDistroUpgrades()) {
            connect(t, SIGNAL(distroUpgrade(PackageKit::Client::UpgradeType, const QString &, const QString &)),
                    this, SLOT(distroUpgrade(PackageKit::Client::UpgradeType, const QString &, const QString &)));
            connect(t, SIGNAL(finished(PackageKit::Transaction::ExitStatus, uint)),
                    this, SLOT(decreaseRunning()));
            increaseRunning();
        }
    }
}

void KpkDistroUpgrade::distroUpgrade(PackageKit::Client::UpgradeType type, const QString &name, const QString &description)
{
    Q_UNUSED(type)
    kDebug() << "Distro upgrade found!" << name << description;
    increaseRunning();
    // TODO kde4.2 does not work with KNotification::Persistent, when it's fixed we move back
    // otherwise we will leak
    KNotification *notify = new KNotification("DistroUpgradeAvailable", 0, KNotification::CloseOnTimeout);

    QString text;
    text =  i18n("Distribution upgrade available") + "<br/>";
    text += "<b>" + name + "</b><br/>";
    text += description;
    notify->setText(text);

    QStringList actions;
    actions << i18n("Start upgrade now");
    notify->setActions(actions);
    connect(notify, SIGNAL(activated(uint)),
            this, SLOT(handleDistroUpgradeAction(uint)));
    connect(notify, SIGNAL(closed()),
            this , SLOT(handleDistroUpgradeActionClosed()));
    notify->sendEvent();
}

void KpkDistroUpgrade::handleDistroUpgradeAction(uint action)
{
    // get the sender cause there might be more than one
    KNotification *notify = (KNotification *) sender();
    switch(action) {
        case 1:
            // Check to see if there isn't another process running
            if (m_distroUpgradeProcess) {
                // if so we BREAK otherwise our running count gets
                // lost, and we leak as we don't close the caller.
                break;
            }
            m_distroUpgradeProcess = new QProcess;
            increaseRunning();
            connect (m_distroUpgradeProcess, SIGNAL(error(QProcess::ProcessError)),
                    this, SLOT(distroUpgradeError(QProcess::ProcessError)));
            connect (m_distroUpgradeProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                    this, SLOT(distroUpgradeFinished(int, QProcess::ExitStatus)));

            m_distroUpgradeProcess->start("/usr/share/PackageKit/pk-upgrade-distro.sh");
//             suppressSleep(true);
            break;
        // perhaps more actions needed in the future
    }
    // in persistent mode we need to manually close it
    notify->close();
}

void KpkDistroUpgrade::handleDistroUpgradeActionClosed()
{
    kDebug();
    decreaseRunning();
}

void KpkDistroUpgrade::distroUpgradeFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    decreaseRunning();
    KNotification *notify = new KNotification("DistroUpgradeFinished");
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        notify->setPixmap(KIcon("security-high").pixmap(64, 64));
        notify->setText(i18n("Distribution upgrade finished. "));
    } else if (exitStatus == QProcess::NormalExit) {
        notify->setPixmap(KIcon("dialog-warning").pixmap(64, 64));
        notify->setText(i18n("Distribution upgrade process exited with code %1.", exitCode));
    }/* else {
        notify->setText(i18n("Distribution upgrade didn't exit normally, the process probably crashed. "));
    }*/
    notify->sendEvent();
    m_distroUpgradeProcess->deleteLater();
    m_distroUpgradeProcess = 0;
//     suppressSleep(false);
}

void KpkDistroUpgrade::distroUpgradeError(QProcess::ProcessError error)
{
    QString text;

    KNotification *notify = new KNotification("DistroUpgradeError");
    switch(error) {
        case QProcess::FailedToStart:
            text = i18n("The distribution upgrade process failed to start.");
            break;
        case QProcess::Crashed:
            text = i18n("The distribution upgrade process crashed some time after starting successfully.") ;
            break;
        default:
            text = i18n("The distribution upgrade process failed with an unknown error.");
            break;
    }
    notify->setPixmap(KIcon("dialog-error").pixmap(64,64));
    notify->setText(text);
    notify->sendEvent();
}

#include "KpkDistroUpgrade.moc"