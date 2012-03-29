/*
   Copyright 2011 Last.fm Ltd.
      - Primarily authored by Michael Coffey

   This file is part of liblastfm.

   liblastfm is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   liblastfm is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with liblastfm.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LASTFM_LIBRARY_H
#define LASTFM_LIBRARY_H

#include "Track.h"
#include "Artist.h"
#include "Album.h"

namespace lastfm
{
    class LASTFM_DLLEXPORT Library
    {
    private:
        Library();

    public:
        static class QNetworkReply* addAlbum( const QList<lastfm::Album>& albums );
        static class QNetworkReply* addArtist( const QList<lastfm::Artist>& artists );
        static class QNetworkReply* addTrack( const lastfm::Track& tracks );

        static class QNetworkReply* getAlbums( const QString& user, const lastfm::Artist& artist = lastfm::Artist(), int limit = -1, int page = -1 );
        static class QNetworkReply* getArtists( const QString& user, int limit = -1, int page = -1 );
        static class QNetworkReply* getTracks( const QString& user, const lastfm::Artist& artist = lastfm::Artist(), int limit = -1, int page = -1 );
        static class QNetworkReply* getTracks( const QString& user, const lastfm::Album& album = lastfm::Album(), int limit = -1, int page = -1 );

        static class QNetworkReply* removeAlbum( const lastfm::Album& album );
        static class QNetworkReply* removeArtist( const lastfm::Artist& artist );
        static class QNetworkReply* removeTrack( const lastfm::Track& track );

        static class QNetworkReply* removeScrobble( const lastfm::Track& track );
    };
}

#endif // LASTFM_LIBRARY_H

