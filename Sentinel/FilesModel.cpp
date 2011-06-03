/***************************************************************************
 *   Copyright (C) 2009-2010 by Daniel Nicoletti                           *
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

#include "FilesModel.h"

#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

#include <KDebug>
#include <KpkIcons.h>
#include <KLocale>
#include <KMimeType>
#include <KIconLoader>

FilesModel::FilesModel(const QStringList &files, const QStringList &mimes, QObject *parent)
: QStandardItemModel(parent),
  m_mimes(mimes)
{
    QList<QUrl> urls;
    foreach (const QString &file, files) {
        urls << file;
    }
    insertFiles(urls);
}

bool FilesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action)
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(parent)
    bool ret = false;
    if (data->hasUrls()) {
        ret = insertFiles(data->urls());
    }
    return ret;
}

bool FilesModel::insertFiles(const QList<QUrl> &urls)
{
    bool ret = false;
    foreach (const QUrl &url, urls) {
        if (files().contains(url.path())) {
            continue;
        }

        QFileInfo fileInfo(url.path());
        if (fileInfo.isFile()) {
            KMimeType::Ptr mime = KMimeType::findByFileContent(url.path());
            foreach (const QString &mimeType, m_mimes) {
                if (mime->is(mimeType)) {
                    ret = true;
/*                    kDebug() << "Found Supported Mime" << mimeType << mime->iconName();*/
                    QStandardItem *item = new QStandardItem(fileInfo.fileName());
                    item->setData(url.path());
                    item->setToolTip(url.path());
                    item->setIcon(KIconLoader::global()->loadMimeTypeIcon(mime->iconName(),
                                                                        KIconLoader::Desktop));
                    appendRow(item);
                    break;
                }
            }
        }
    }
    return ret;
}

QStringList FilesModel::mimeTypes() const
{
    return QStringList() << "text/uri-list";
}

Qt::DropActions FilesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

Qt::ItemFlags FilesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList FilesModel::files() const
{
    QStringList ret;
    for (int i = 0; i < rowCount(); ++i) {
        ret << item(i)->data().toString();
    }
    return ret;
}

#include "FilesModel.moc"