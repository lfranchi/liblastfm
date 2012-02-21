/*
   Copyright 2009-2010 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include "Album.h"
#include "Artist.h"
#include "User.h"
#include "UrlBuilder.h"
#include "XmlQuery.h"
#include "ws.h"
#include <QFile>
#include <QStringList>
#include <QTimer>

using lastfm::Album;
using lastfm::Artist;
using lastfm::Mbid;

Album::Album()
{}

Album::Album( Mbid mbid )
    : m_mbid( mbid )
{}

Album::Album( Artist artist, QString title )
    : m_artist( artist ), m_title( title )
{}

bool
Album::operator==( const Album& that ) const
{
    return m_title == that.m_title && m_artist == that.m_artist;
}

bool
Album::operator!=( const Album& that ) const
{
    return m_title != that.m_title || m_artist != that.m_artist;
}

Album::operator QString() const
{
    return title();
}

QString
Album::title() const
{
    return m_title;
}

Artist
Album::artist() const
{
    return m_artist;
}

Mbid
Album::mbid() const
{
    return m_mbid;
}

bool
Album::isNull() const
{
    return m_title.isEmpty() && m_mbid.isNull();
}


QNetworkReply*
lastfm::Album::getInfo( const QString& username) const
{
    QMap<QString, QString> map;
    map["method"] = "album.getInfo";
    map["artist"] = m_artist;
    map["album"] = m_title;
    if (!username.isEmpty()) map["username"] = username;
    if (!lastfm::ws::SessionKey.isEmpty()) map["sk"] = lastfm::ws::SessionKey;
    return lastfm::ws::get(map);
}


QNetworkReply*
lastfm::Album::getTags() const
{
    QMap<QString, QString> map;
    map["method"] = "album.getTags";
    map["artist"] = m_artist;
    map["album"] = m_title;
    return lastfm::ws::get(map);
}


QNetworkReply*
lastfm::Album::share( const QStringList& recipients, const QString& message, bool isPublic ) const
{
    QMap<QString, QString> map;
    map["method"] = "album.share";
    map["artist"] = m_artist;
    map["album"] = m_title;
    map["recipient"] = recipients.join(",");
    map["public"] = isPublic ? "1" : "0";
    if (message.size()) map["message"] = message;
    return lastfm::ws::post(map);
}


QUrl
lastfm::Album::www() const
{
    return lastfm::UrlBuilder( "music" ).slash( m_artist ).slash( m_title ).url();
}


QNetworkReply*
lastfm::Album::addTags( const QStringList& tags ) const
{
    if (tags.isEmpty())
        return 0;

    QMap<QString, QString> map;
    map["method"] = "album.addTags";
    map["artist"] = m_artist;
    map["album"] = m_title;
    map["tags"] = tags.join( QChar(',') );
    return lastfm::ws::post(map);
}
