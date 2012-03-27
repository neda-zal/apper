/* This file is part of Apper
 *
 * Copyright (C) 2012 Matthias Klumpp <matthias@tenstral.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <KAboutData>
#include <KCmdLineArgs>
#include <KDebug>
#include <KApplication>
#include <glib-object.h>

#include "SetupWizard.h"

int main(int argc, char** argv)
{
    KAboutData aboutData("appsetup-kde", 0, ki18n("KDE Application Installer"), "0.1",
                         ki18n("KDE Application Installer"), KAboutData::License_GPL,
                         ki18n("(C) 2012, Matthias Klumpp"));

    aboutData.addAuthor(ki18nc("@info:credit", "Daniel Nicoletti"), ki18n("Developer"),
                        "dantti12@gmail.com");
    aboutData.addAuthor(ki18nc("@info:credit", "Matthias Klumpp"), ki18n("Developer"),
                        "matthias@tenstral.net");
    aboutData.setProductName("apper/listaller");

    KCmdLineArgs::init(argc, argv, &aboutData);
    // Add --debug as commandline option
    KCmdLineOptions options;
    options.add("debug", ki18n("Show debugging information"));
    KCmdLineArgs::addCmdLineOptions(options);

    // Initialize GObject type system
    g_type_init();

    KApplication app;
    SetupWizard *wizard = new SetupWizard();
    wizard->show();
    return app.exec();
}
