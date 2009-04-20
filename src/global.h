/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#ifndef LASTFM_GLOBAL_H
#define LASTFM_GLOBAL_H

/** LASTFM_EXPORTs symbols when compiled as part of the lib
  * LASTFM_IMPORTs when included from some other target */
#if defined(_WIN32) || defined(WIN32)
    #define LASTFM_EXPORT __declspec(dllLASTFM_EXPORT)
    #define LASTFM_IMPORT __declspec(dllLASTFM_IMPORT)
#elif __GNUC__ >= 4
    // GCC is just annoying, yes both should be default
    #define LASTFM_EXPORT __attribute__ ((visibility("default")))
    #define LASTFM_IMPORT __attribute__ ((visibility("default")))
#else
    #define LASTFM_EXPORT
    #define LASTFM_IMPORT
#endif

#ifdef _RADIO_DLLEXPORT
    #define LASTFM_RADIO_DLLEXPORT LASTFM_EXPORT
#else
    #define LASTFM_RADIO_DLLEXPORT LASTFM_IMPORT
#endif

#ifdef _FINGERPRINT_DLLEXPORT
    #define LASTFM_FINGERPRINT_DLLEXPORT LASTFM_EXPORT
#else
    #define LASTFM_FINGERPRINT_DLLEXPORT LASTFM_IMPORT
#endif

#ifdef _WS_DLLEXPORT
    #define LASTFM_WS_DLLEXPORT LASTFM_EXPORT
#else
    #define LASTFM_WS_DLLEXPORT LASTFM_IMPORT
#endif

#ifdef _TYPES_DLLEXPORT
    #define LASTFM_TYPES_DLLEXPORT LASTFM_EXPORT
#else
    #define LASTFM_TYPES_DLLEXPORT LASTFM_IMPORT
#endif

#ifdef _CORE_DLLEXPORT
    #define LASTFM_CORE_DLLEXPORT LASTFM_EXPORT
#else
    #define LASTFM_CORE_DLLEXPORT LASTFM_IMPORT
#endif

#ifdef _SCROBBLE_DLLEXPORT
    #define LASTFM_SCROBBLE_DLLEXPORT LASTFM_EXPORT
#else
    #define LASTFM_SCROBBLE_DLLEXPORT LASTFM_IMPORT
#endif


#include <QMetaEnum>
#include <QString>

namespace lastfm
{
    /** http://labs.trolltech.com/blogs/2008/10/09/coding-tip-pretty-printing-enum-values
      * Tips for making this take a single parameter welcome! :)
      * 
      * eg. lastfm::qMetaEnumString<QNetworkReply>( error, "NetworkError" );
      */
    template <typename T> static inline QString qMetaEnumString( int enum_value, const char* enum_name )
    {
        QMetaObject meta = T::staticMetaObject;
        for (int i=0; i < meta.enumeratorCount(); ++i)
        {
            QMetaEnum m = meta.enumerator(i);
            if (m.name() == QLatin1String(enum_name))
                return QLatin1String(m.valueToKey(enum_value));
        }
        return QString("Unknown enum value for \"%1\": %2").arg( enum_name ).arg( enum_value );
    }


	enum ImageSize
	{
		Small = 0,
		Medium = 1,
		Large = 2, /** seemingly 174x174 */
        ExtraLarge = 3
	};
	
	
	//convenience
    class Album;
    class Artist;
    class Audioscrobbler;
    class AuthenticatedUser;
    class Fingerprint;
    class FingerprintId;
    class Mbid;
    class Playlist;
    class User;
    class Tag;
    class Track;
    class Xspf;
}


#ifdef LASTFM_COLLAPSE_NAMESPACE
using lastfm::Album;
using lastfm::Artist;
using lastfm::Audioscrobbler;
using lastfm::AuthenticatedUser;
using lastfm::Fingerprint;
using lastfm::FingerprintId;
using lastfm::Mbid;
using lastfm::Playlist;
using lastfm::User;
using lastfm::Tag;
using lastfm::Track;
using lastfm::Xspf;
#endif


//convenience
class WsReply;
class QNetworkReply;


//convenience for development
#include <QDebug>

#endif //LASTFM_GLOBAL_H
