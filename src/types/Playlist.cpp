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

#include "Playlist.h"
#include "Track.h"
#include "../ws/ws.h"


QNetworkReply*
lastfm::Playlist::addTrack( const Track& t ) const
{
    QMap<QString, QString> map;
    map["method"] = "playlist.addTrack";
    map["playlistID"] = m_id;
    map["artist"] = t.artist();
    map["track"] = t.title();
    return lastfm::ws::post(map);
}


QNetworkReply*
lastfm::Playlist::fetch() const
{
    return fetch( QUrl("lastfm://playlist/" + QString::number( m_id )) );
}


QNetworkReply* //static
lastfm::Playlist::fetch( const QUrl& url )
{
    QMap<QString, QString> map;
    map["method"] = "playlist.fetch";
    map["playlistURL"] = url.toString();
    return lastfm::ws::get(map);
}


QNetworkReply* //static
lastfm::Playlist::create( const QString& title, const QString& description /*=""*/ )
{
    QMap<QString, QString> map;
    map["method"] = "playlist.create";
    map["title"] = title;
    if (description.size()) 
        map["description"] = description;
    return lastfm::ws::post(map);
}
