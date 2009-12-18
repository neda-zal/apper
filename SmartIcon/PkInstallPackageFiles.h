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
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#ifndef PK_INSTALL_PACKAGE_FILES_H
#define PK_INSTALL_PACKAGE_FILES_H

#include <KpkTransaction.h>
#include "KpkAbstractTask.h"

#include <QPackageKit>

using namespace PackageKit;

class KpkSimulateModel;
class PkInstallPackageFiles : public KpkAbstractTask
{
Q_OBJECT
public:
    PkInstallPackageFiles(uint xid,
                          const QStringList &files,
                          const QString &interaction,
                          const QDBusMessage &message,
                          QWidget *parent = 0);
    ~PkInstallPackageFiles();

public slots:
    void start();

private slots:
    void installFilesFinished(KpkTransaction::ExitStatus status);
    void simulateFinished(PackageKit::Transaction::ExitStatus status, uint runtime);
//     void installFinished(PackageKit::Transaction::ExitStatus status, uint runtime);

private:
    void installFiles();
    QHash <KpkTransaction *, QStringList> m_transactionFiles;
    KUrl::List m_urls;
    QStringList m_files;

    KpkSimulateModel *m_installFilesModel;
};

#endif