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

#include "KPackageKitSmartIcon.h"
#include <version.h>

#include <KDebug>
#include <KConfig>
#include <KLocale>
#include <KAboutData>
#include <KCmdLineArgs>

int main(int argc, char **argv)
{
    KAboutData about(
	"kpackagekit-smart-icon", "kpackagekit", ki18n("kpackagekit-smart-icon"),
	KPK_VERSION, ki18n("KPackageKit-Smart-Icon application for showing running transactions"),
	KAboutData::License_GPL, ki18n("(C) 2008 Daniel Nicoletti"),
	KLocalizedString(), "http://www.packagekit.org/");

    about.addAuthor(ki18n("Daniel Nicoletti"), KLocalizedString(), "dantti85-pk@yahoo.com.br","http://www.packagekit.org" );

    about.addCredit(ki18n("Adrien Bustany"), ki18n("libpackagekit-qt and other stuff"),"@");

    KCmdLineArgs::init(argc, argv, &about);

    if (!kpackagekit::KPackageKit_Smart_Icon::start())
    {
	kDebug() << "KPackageKit-Smart-Icon is already running!";
	return 0;
    }

    kpackagekit::KPackageKit_Smart_Icon app;
    app.exec();
    return 0;
}
