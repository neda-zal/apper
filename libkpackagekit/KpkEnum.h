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

#ifndef KPKENUM_H
#define KPKENUM_H

#include <config.h>

#ifdef HAVE_AUTOREMOVE
#define AUTOREMOVE true
#else
#define AUTOREMOVE false
#endif // HAVE_AUTOREMOVE

namespace KpkEnum {

    typedef enum {
            None,
            Security,
            All
    } AutoUpdate;
    const int AutoUpdateDefault = None;

    typedef enum {
            Never   =       0,
            Hourly  =    3600,
            Daily   =   86400,
            Weekly  =  604800,
            Monthly = 2628000
    } TimeInterval;
    const int TimeIntervalDefault = Weekly;

}

#endif
