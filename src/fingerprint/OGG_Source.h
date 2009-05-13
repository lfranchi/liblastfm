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

#ifndef __OGG_SOURCE
#define __OGG_SOURCE

#include <vorbis/vorbisfile.h>
#include <QString>

#include "FileSourceInterface.h"

class OGG_Source : public FileSourceInterface
{
public:
    // ctor
    OGG_Source(const QString& fileName);
    virtual ~OGG_Source();

    virtual void getInfo(int& lengthSecs, int& samplerate, int& bitrate, int& nchannels);
    virtual void init();

    // return a chunk of PCM data from the ogg file
    virtual int updateBuffer(signed short* pBuffer, size_t bufferSize);

    virtual void skip(const int mSecs);
    virtual void skipSilence(double silenceThreshold = 0.0001);

    //QString getMbid();

    bool  eof() const { return m_eof; }

private:
    OggVorbis_File m_vf;
    QString m_fileName;
    int m_channels;
    int m_samplerate;
    bool m_eof;
};

#endif

