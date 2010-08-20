/***************************************************************************
 *   Copyright (C) 2010 by Daniel Nicoletti                                *
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

#include "ProgressView.h"

#include <QVBoxLayout>
#include <QScrollBar>

#include <KLocale>
#include <KConfigGroup>
#include <KDebug>

#include "TransactionDelegate.h"

ProgressView::ProgressView(QWidget *parent)
 : QWidget(parent),
   m_keepScrollBarBottom(true)
{
    m_model = new QStandardItemModel(this);

    m_packageView = new QTreeView(this);
    m_packageView->setModel(m_model);
    m_packageView->setItemDelegate(new TransactionDelegate(this));
    m_packageView->setRootIsDecorated(false);
    m_packageView->setHeaderHidden(true);
    m_packageView->setSelectionMode(QAbstractItemView::NoSelection);
    m_packageView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_packageView->verticalScrollBar()->value();

    m_scrollBar = m_packageView->verticalScrollBar();
    connect(m_scrollBar, SIGNAL(sliderMoved(int)),
            this, SLOT(followBottom(int)));
    connect(m_scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(followBottom(int)));
    connect(m_scrollBar, SIGNAL(rangeChanged(int, int)),
            this, SLOT(rangeChanged(int, int)));

    m_label = new QLabel(this);
    m_label->hide();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_packageView);
    layout->addWidget(m_label);

    KConfig config("KPackageKit");
    KConfigGroup transactionDialog(&config, "TransactionDialog");

    resize(width(), transactionDialog.readEntry("detailsHeight", QWidget::height()));
}

ProgressView::~ProgressView()
{
    KConfig config("KPackageKit");
    KConfigGroup transactionDialog(&config, "TransactionDialog");
    transactionDialog.writeEntry("detailsHeight", height());
}

void ProgressView::setSubProgress(int value)
{
    // TODO change PackageKit to emit the package progress on another signal.
    QStandardItem *item;
    QList<QStandardItem *> packages = m_model->findItems(m_lastPackageId);
    item = m_model->item(m_model->rowCount() - 1);
    if (!packages.isEmpty() &&
        (item = packages.last()) &&
        item->data(RoleFinished).toBool() == false) {
        // if the progress is unknown (101), make it empty
        if (value == 101) {
            value = 0;
        }
        if (item->data(RoleProgress).toInt() != value) {
            item->setData(value, RoleProgress);
        }
    }
}

void ProgressView::clear()
{
    m_model->clear();
}

void ProgressView::currentPackage(QSharedPointer<PackageKit::Package> p)
{
    if (!p->id().isEmpty()) {
        m_lastPackageId = p->id();

        QStandardItem *item;
        QList<QStandardItem *> packages = m_model->findItems(p->id());
        // If there is alread some packages check to see if it has
        // finished, if the progress is 100 create a new item for the next task
        if (!packages.isEmpty() &&
            (item = packages.last()) &&
            item->data(RoleFinished).toBool() == false)
        {
            // if the item status (info) changed update it
            if (item->data(RoleInfo).toInt() != p->info()) {
                // If the package task has finished set progress to 100
                if (p->info() == Enum::InfoFinished) {
                    itemFinished(item);
                } else {
                    item->setData(p->info(), RoleInfo);
                }
            }
        } else {
            QList<QStandardItem *> items;
            // It's a new package create it and append it
            item = new QStandardItem(p->id());
            item->setData(p->info(), RoleInfo);
            item->setData(0,         RoleProgress);
            item->setData(false,     RoleFinished);
            items << item;

            item = new QStandardItem(p->name());
            item->setToolTip(p->name());
            items << item;

            item = new QStandardItem(p->summary());
            item->setToolTip(p->summary());
            items << item;

            m_model->appendRow(items);
        }
    }
}

void ProgressView::itemFinished(QStandardItem *item)
{
    // Point to the item before it
    int count = item->row() - 1;

    // Find the last finished item
    bool found = false;
    while (count >= 0) {
        // Put it after the finished item
        // so that running items can be kept
        // at the bottom
        if (m_model->item(count)->data(RoleFinished).toBool()) {
            // make sure it won't end in the same position
            if (count + 1 != item->row()) {
                QList<QStandardItem*> items;
                items = m_model->takeRow(item->row());
                m_model->insertRow(count + 1, items);
            }
            found = true;
            break;
        }
        --count;
    }

    // If it's not at the top of the list
    // and no FINISHED Item was found move it there
    if (!found && item->row() != 0) {
        QList<QStandardItem*> items;
        items = m_model->takeRow(item->row());
        m_model->insertRow(0, items);
    }

    item->setData(100,  RoleProgress);
    item->setData(true, RoleFinished);
}

void ProgressView::followBottom(int value)
{
    // If the user moves the slider to the bottom
    // keep it there as the list expands
    m_keepScrollBarBottom = value == m_scrollBar->maximum();
}

void ProgressView::rangeChanged(int min, int max)
{
    Q_UNUSED(min)
    if (m_keepScrollBarBottom && m_scrollBar->value() != max) {
        m_scrollBar->setValue(max);
    }
}

#include "ProgressView.moc"
