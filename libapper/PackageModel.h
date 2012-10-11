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

#ifndef PACKAGE_MODEL_H
#define PACKAGE_MODEL_H

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMetaObject>
#include <KIcon>

#include <Transaction>

class KDE_EXPORT PackageModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY changed)
public:
    enum {
        NameCol = 0,
        VersionCol,
        CurrentVersionCol,
        ArchCol,
        OriginCol,
        SizeCol,
        ActionCol
    };
    enum {
        SortRole = Qt::UserRole,
        NameRole,
        SummaryRole,
        VersionRole,
        ArchRole,
        IconRole,
        IdRole,
        CheckStateRole,
        InfoRole,
        ApplicationId,
        IsPackageRole,
        PackageName,
        InfoIconRole
    };
    typedef struct {
        QString    displayName;
        QString    version;
        QString    arch;
        QString    repo;
        QString    packageID;
        QString    summary;
        PackageKit::Transaction::Info info;
        QString    icon;
        QString    appId;
        QString    currentVersion;
        bool       isPackage;
        double     size;
    } InternalPackage;

    explicit PackageModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool allSelected() const;
    Q_INVOKABLE QStringList selectedPackagesToInstall() const;
    Q_INVOKABLE QStringList selectedPackagesToRemove() const;
    unsigned long downloadSize() const;
    void clear();
    /**
     * This removes all selected packages that are not in the model
     */
    void clearSelectedNotPresent();

    bool checkable() const;
    void setCheckable(bool checkable);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

public slots:
    void addPackage(PackageKit::Transaction::Info info, const QString &packageID, const QString &summary, bool selected = false);
    void addPackages(const QStringList &packages,
                     bool selected = false);
    void addSelectedPackage(PackageKit::Transaction::Info info, const QString &packageID, const QString &summary);
    void rmSelectedPackage(const PackageModel::InternalPackage &package);

    void setAllChecked(bool checked);
    void checkPackage(const PackageModel::InternalPackage &package,
                      bool emitDataChanged = true);
    void uncheckPackage(const PackageModel::InternalPackage &package,
                        bool forceEmitUnchecked = false,
                        bool emitDataChanged = true);
    bool hasChanges() const;

    void uncheckInstalledPackages();
    void uncheckAvailablePackages();

    void finished();

    void fetchSizes();
    void fetchSizesFinished();
    void updateSize(const QString &packageID,
                    const QString &license,
                    PackageKit::Transaction::Group group,
                    const QString &detail,
                    const QString &url,
                    qulonglong size);

    void fetchCurrentVersions();
    void fetchCurrentVersionsFinished();
    void updateCurrentVersion(PackageKit::Transaction::Info info, const QString &packageID, const QString &summary);

    void getUpdates(bool fetchCurrentVersions, bool selected);

signals:
    void changed(bool value);
    void packageChecked(const PackageModel::InternalPackage &package);
    void packageUnchecked(const PackageModel::InternalPackage &package);

private:
    bool containsChecked(const QString &pid) const;

    int m_packageCount;
    bool                            m_checkable;
    QPixmap                         m_installedEmblem;
    QVector<InternalPackage>        m_packages;
    QHash<QString, InternalPackage> m_checkedPackages;
    PackageKit::Transaction *m_fetchSizesTransaction;
    PackageKit::Transaction *m_fetchInstalledVersionsTransaction;
};

#endif
