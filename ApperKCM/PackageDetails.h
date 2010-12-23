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

#ifndef PACKAGE_DETAILS_H
#define PACKAGE_DETAILS_H

#include <QPackageKit>

#include <KPixmapSequenceOverlayPainter>
#include <KJob>

#include <QWidget>
#include <QSortFilterProxyModel>

#include "ui_PackageDetails.h"

using namespace PackageKit;

class KpkPackageModel;
class PackageDetails : public QWidget, Ui::PackageDetails
{
Q_OBJECT
public:
    enum FadeWidget {
        FadeNone       = 0x0,
        FadeStacked    = 0x1,
        FadeScreenshot = 0x2
    };
    Q_DECLARE_FLAGS(FadeWidgets, FadeWidget)
    PackageDetails(QWidget *parent = 0);
    ~PackageDetails();

    void setPackage(const QModelIndex &index);
    void hidePackageVersion(bool hide);
    void hidePackageArch(bool hide);

public slots:
    void hide();

signals:
    void ensureVisible(const QModelIndex &index);

private slots:
    void on_screenshotL_clicked();
    void actionActivated(QAction *action);
    void description(const QSharedPointer<PackageKit::Package> &package);
    void files(QSharedPointer<PackageKit::Package> package, const QStringList &files);
    void finished();
    void resultJob(KJob *);

    void display();

private:
    void fadeOut(FadeWidgets widgets);
    void setupDescription();
    QVector<QPair<QString, QString> > locateApplication(const QString &_relPath, const QString &menuId) const;

    QActionGroup *m_actionGroup;
    QModelIndex   m_index;
    QSharedPointer<PackageKit::Package> m_package;
    QString m_appName;

    QParallelAnimationGroup       *m_expandPanel;
    KPixmapSequenceOverlayPainter *m_busySeq;

    QPropertyAnimation *m_fadeStacked;
    QPropertyAnimation *m_fadeScreenshot;
    bool m_display;
    bool m_hideVersion;
    bool m_hideArch;

    // We need a copy of prety much every thing
    // we have, so that we update only when we are
    // totaly transparent this way the user
    // does not see the ui flicker
    Transaction *m_transaction;
    bool         m_hasDetails;
    QString      m_currentText;
    QPixmap      m_currentIcon;
    QString      m_appId;
    QString      m_packageId;

    // file list buffer
    bool         m_hasFileList;
    QStringList  m_currentFileList;

    // GetDepends buffer
    bool m_hasDepends;
    KpkPackageModel *m_dependsModel;
    QSortFilterProxyModel *m_dependsProxy;

    // GetRequires buffer
    bool m_hasRequires;
    KpkPackageModel *m_requiresModel;
    QSortFilterProxyModel *m_requiresProxy;

    // Screen shot buffer
    QString      m_currentScreenshot;
    QHash<QString, QString> m_screenshotPath;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PackageDetails::FadeWidgets)

#endif