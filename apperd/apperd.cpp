/***************************************************************************
 *   Copyright (C) 2008-2011 by Daniel Nicoletti                           *
 *   dantti12@gmail.com                                                    *
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

#include "apperd.h"
#include "ApperdThread.h"

#include <KGenericFactory>
#include <KDebug>

K_PLUGIN_FACTORY(ApperFactory, registerPlugin<ApperD>();)
K_EXPORT_PLUGIN(ApperFactory("apperd"))

ApperD::ApperD(QObject *parent, const QList<QVariant> &) :
    KDEDModule(parent)
{
//    m_thread = new QThread(this);
    m_apperThread = new ApperdThread;
//    m_apperThread->moveToThread(m_thread);
//    m_thread->start();

    // Make all our init code run on the thread since
    // the DBus calls were made blocking
    QTimer::singleShot(0, m_apperThread, SLOT(init()));
}

ApperD::~ApperD()
{
    // delete the apper thread code, don't use delete later since it has moved
    // to the stopped thread
    m_apperThread->deleteLater();
//    m_thread->quit();
//    m_thread->wait();
//    delete m_thread;
}
