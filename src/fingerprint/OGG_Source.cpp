/***************************************************************************
 *   Copyright (C) 2008-2009 John Stamp <jstamp@users.sourceforge.net>     *
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

#include "OGG_Source.h"

#include <QFile>
#include <cassert>
#include <errno.h>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <cstdlib>

// These specify the output format
static const int wordSize = 2; // 16 bit output
static const int isSigned = 1;
#if __BIG_ENDIAN__
static const int isBigEndian = 1;
#else
static const int isBigEndian = 0;
#endif


OGG_Source::OGG_Source(const QString& fileName)
    : m_fileName(fileName)
    , m_channels( 0 )
    , m_samplerate( 0 )
    , m_eof( false )
{
    memset( &m_vf, 0, sizeof(m_vf) );
}

// ---------------------------------------------------------------------

OGG_Source::~OGG_Source()
{
    // ov_clear() also closes the file
    ov_clear( &m_vf );
}

// ---------------------------------------------------------------------

void OGG_Source::init()
{
    if ( m_vf.datasource )
    {
        std::cerr << "Warning: file already appears to be open";
        return;
    }

    FILE *fp = fopen(QFile::encodeName(m_fileName), "rb" );
    if( !fp )
        throw std::runtime_error( "ERROR: Cannot open ogg file!" );

    // See the warning about calling ov_open on Windows
    if ( ov_test_callbacks( fp, &m_vf, NULL, 0, OV_CALLBACKS_DEFAULT ) < 0 )
    {
        fclose( fp );
        throw std::runtime_error( "ERROR: This is not an ogg vorbis file!" );
    }

    ov_test_open( &m_vf );

    // Don't fingerprint files with more than one logical bitstream
    // They most likely contain more than one track
    if ( ov_streams( &m_vf ) != 1 )
        throw std::runtime_error( "ERROR: ogg file contains multiple bitstreams" );

    m_channels = ov_info( &m_vf, 0 )->channels;
    m_samplerate = static_cast<int>(ov_info( &m_vf, 0 )->rate);
    m_eof = false;
}

// ---------------------------------------------------------------------

/*QString OGG_Source::getMbid()
{
    // Likely more than one track
    // Return nothing
    if ( ov_streams( &m_vf ) != 1 )
        return QString();

    vorbis_comment *vc = ov_comment( &m_vf, -1 );
    for ( int i = 0; i < vc->comments; i++ )
    {
        QString comment = vc->user_comments[i];
        int delim = comment.indexOf( '=' );
        if ( delim >= 0 )
        {
            QString key( comment.left(delim) );
            if ( key.toLower() == "musicbrainz_trackid" )
            {
                QString val = comment.mid(delim+1);
                if ( val.length() == 36 )
                {
                    return val;
                }
            }
        }
    }

    return QString();
}*/

// ---------------------------------------------------------------------

void OGG_Source::getInfo( int& lengthSecs, int& samplerate, int& bitrate, int& nchannels)
{
    // stream info
    nchannels = ov_info( &m_vf, -1 )->channels;
    samplerate = static_cast<int>(ov_info( &m_vf, -1 )->rate);
    lengthSecs = static_cast<int>(ov_time_total( &m_vf, -1 ) + 0.5);
    bitrate = static_cast<int>(ov_bitrate( &m_vf, -1 ));
}

// ---------------------------------------------------------------------

void OGG_Source::skip( const int mSecs )
{
    if ( mSecs < 0 )
        return;

    double ts = mSecs / 1000.0 + ov_time_tell( &m_vf );
    ov_time_seek( &m_vf, ts );
}

// ---------------------------------------------------------------------

void OGG_Source::skipSilence(double silenceThreshold /* = 0.0001 */)
{
    silenceThreshold *= static_cast<double>( std::numeric_limits<short>::max() );

    char sampleBuffer[4096];
    int bs = 0;
    for (;;)
    {
        long charReadBytes = ov_read( &m_vf, sampleBuffer, 4096, isBigEndian, wordSize, isSigned, &bs );

        // eof
        if ( !charReadBytes )
        {
            m_eof = true;
            break;
        }
        if ( charReadBytes < 0 )
        {
            // a bad bit of data: OV_HOLE || OV_EBADLINK
            continue;
        }
        else if ( charReadBytes > 0 )
        {
            double sum = 0;
            int16_t *buf = reinterpret_cast<int16_t*>(sampleBuffer);
            switch ( m_channels )
            {
                case 1:
                    for (long j = 0; j < charReadBytes/wordSize; j++)
                        sum += abs( buf[j] );
                    break;
                case 2:
                    for (long j = 0; j < charReadBytes/wordSize; j+=2)
                        sum += abs( (buf[j] >> 1) + (buf[j+1] >> 1) );
                    break;
            }
            if ( sum >= silenceThreshold * static_cast<double>(charReadBytes/wordSize/m_channels) )
                break;
        }
    }
}

// ---------------------------------------------------------------------

int OGG_Source::updateBuffer( signed short *pBuffer, size_t bufferSize )
{
    char buf[ bufferSize * wordSize ];
    int bs = 0;
    size_t charwrit = 0; //number of samples written to the output buffer

    for (;;)
    {
        long charReadBytes = ov_read( &m_vf, buf, static_cast<int>(bufferSize * wordSize - charwrit),
                                      isBigEndian, wordSize, isSigned, &bs );
        if ( !charReadBytes )
        {
            m_eof = true;
            break; // nothing else to read
        }

        // Don't really need this though since we're excluding files that have
        // more than one logical bitstream
        if ( bs != 0 )
        {
            vorbis_info *vi = ov_info( &m_vf, -1 );
            if ( m_channels != vi->channels || m_samplerate != vi->rate )
            {
                std::cerr << "Files that change channel parameters or samplerate are currently not supported" << std::endl;
                return 0;
            }
        }

        if( charReadBytes < 0 )
        {
            std::cerr << "Warning: corrupt section of data, attempting to continue..." << std::endl;
            continue;
        }

        char* pBufferIt = reinterpret_cast<char*>(pBuffer) + charwrit;
        charwrit += charReadBytes;

        assert( charwrit <= bufferSize * wordSize );
        memcpy( pBufferIt, buf, charReadBytes );

      if (charwrit == bufferSize * wordSize)
         return static_cast<int>(charwrit/wordSize);
   }

   return static_cast<int>(charwrit/wordSize);
}

// -----------------------------------------------------------------------------

