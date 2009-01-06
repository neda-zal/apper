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

#include <KGlobal>
#include <KStartupInfo>
#include <KCmdLineArgs>
#include <KStandardDirs>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <QStringList>

#include "KPackageKitSmartIcon.h"

namespace kpackagekit {

KPackageKit_Smart_Icon::KPackageKit_Smart_Icon()
 : KUniqueApplication()
{
    // this enables not quitting when closing a transaction ui
    setQuitOnLastWindowClosed(false);

    m_closeT = new QTimer(this);
    m_trayIcon = new KpkTransactionTrayIcon(this);
    connect(m_trayIcon, SIGNAL( appClose(int) ), m_closeT, SLOT( start(int) ) );
    connect(m_trayIcon, SIGNAL( cancelClose() ), m_closeT, SLOT( stop() ) );
    connect(m_closeT, SIGNAL( timeout( ) ), this, SLOT( quit() ) );

    // This MUST be called after connecting all the signals or slots!
    m_trayIcon->checkTransactionList();
}

KPackageKit_Smart_Icon::~KPackageKit_Smart_Icon()
{
}

}

#include "KPackageKitSmartIcon.moc"
