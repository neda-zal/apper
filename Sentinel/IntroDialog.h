/***************************************************************************
 *   Copyright (C) 2008-2010 by Daniel Nicoletti                           *
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

#ifndef INTRO_DIALOG_H
#define INTRO_DIALOG_H

#include <QWidget>
#include <QAbstractItemModel>

namespace Ui {
    class IntroDialog;
}

class FilesModel;
class IntroDialog : public QWidget
{
    Q_OBJECT
public:
    IntroDialog(QWidget *parent = 0);
    ~IntroDialog();

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setModel(QAbstractItemModel *model);

private:
    Ui::IntroDialog *ui;
};

#endif