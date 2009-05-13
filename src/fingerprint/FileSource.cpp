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

#include "FileSource.h"
#include "MP3_Source_Qt.h"
#include "OGG_Source.h"

#include <QStringList>

FileSourceInterface * FileSource::createFileSource( const QString& fileName )
{
    int fType = fileSourceType( fileName );

    switch ( fType )
    {
        case MP3:
            return new MP3_Source(fileName);
            break;
        case OGG:
            return new OGG_Source(fileName);
            break;
        default:
           return NULL;
    } 
}

int FileSource::fileSourceType( const QString& fileName )
{
    QStringList parts = fileName.split( "." );
    QString extension;
    if ( parts.size() > 1 )
        extension = parts.last();

    // Let's be trusting about extensions
    if ( extension.toLower() == "mp3" )
        return MP3;
    else if ( extension.toLower() == "ogg" )
        return OGG;

    // So much for relying on extensions.  Let's try file magic instead.
    FILE *fp = NULL;
    unsigned char header[35];

    fp = fopen(QFile::encodeName(fileName), "rb");
    if ( !fp )
    {
        return UNKNOWN;
    }
    int fType = UNKNOWN;
    fread( header, 1, 35, fp );

    // Some formats can have ID3 tags (or not), so let's just
    // get them out of the way first before we check what we have.
    if ( memcmp( header, "ID3", 3) == 0 )
    {
        int tagsize = 0;
        /* high bit is not used */
        tagsize = (header[6] << 21) | (header[7] << 14) |
            (header[8] <<  7) | (header[9] <<  0);

        tagsize += 10;
        fseek( fp, tagsize, SEEK_SET );
        fread( header, 1, 35, fp );
    }

    if ( (header[0] == 0xFF) && ((header[1] & 0xFE) == 0xFA ) )
    {
        fType = MP3;
    }
    else if ( memcmp(header, "OggS", 4) == 0 )
    {
        if ( memcmp(&header[29], "vorbis", 6) == 0 )
        {
            // ogg vorbis (.ogg)
            fType = OGG;
        }
    }

    fclose(fp);
    return fType;
}
