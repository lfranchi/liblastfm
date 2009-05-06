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

#include "Album.h"
#include "Artist.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../core/XmlQuery.h"
#include "../ws/ws.h"
#include <QFile>
#include <QStringList>
#include <QTimer>


QNetworkReply*
lastfm::Album::getInfo() const
{
    QMap<QString, QString> map;
    map["method"] = "album.getInfo";
    map["artist"] = m_artist;
    map["album"] = m_title;
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
lastfm::Album::share( const User& recipient, const QString& message )
{
    QMap<QString, QString> map;
    map["method"] = "album.share";
    map["artist"] = m_artist;
    map["album"] = m_title;
    map["recipient"] = recipient;
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
