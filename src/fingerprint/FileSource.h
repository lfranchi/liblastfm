/***************************************************************************
 *   Copyright (C) 2009 John Stamp <jstamp@users.sourceforge.net>          *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef __FILESOURCE_H
#define __FILESOURCE_H

#include "FileSourceInterface.h"

// ----------------------------------------------------------------------- ------

class FileSource
{
public:
    static FileSourceInterface * createFileSource( const QString& fileName );
    static int fileSourceType( const QString& fileName );
    enum { UNKNOWN = 0,
           MP3,
           OGG,
           FLAC,
           AAC
    };
};

#endif
