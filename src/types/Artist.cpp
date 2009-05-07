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

#include "Artist.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../core/XmlQuery.h"
#include "../ws/ws.h"
#include <QStringList>
using lastfm::Artist;
using lastfm::User;


QMap<QString, QString> //private
Artist::params( const QString& method ) const
{
    QMap<QString, QString> map;
    map["method"] = "artist."+method;
    map["artist"] = m_name;
    return map;
}

QNetworkReply*
Artist::share( const User& user, const QString& message )
{
    QMap<QString, QString> map = params("share");
    map["recipient"] = user;
    if (message.size()) map["message"] = message;
    return lastfm::ws::post(map);
}


QUrl 
Artist::www() const
{
    return UrlBuilder( "music" ).slash( Artist::name() ).url();
}

QNetworkReply* 
Artist::getInfo() const
{
    return ws::get( params("getInfo") );
}


QNetworkReply* 
Artist::getTags() const
{
    return ws::get( params("getTags") );
}

QNetworkReply* 
Artist::getTopTags() const
{
    return ws::get( params("getTopTags") );
}


QNetworkReply* 
Artist::getSimilar() const
{
    return ws::get( params("getSimilar") );
}


QNetworkReply* 
Artist::search( int limit ) const
{
    QMap<QString, QString> map = params("search");
    if (limit > 0) map["limit"] = QString::number(limit);
    return ws::get(map);
}


QMap<int, QString> /* static */
Artist::getSimilar( QNetworkReply* r )
{
    QMap<int, QString> artists;
    try
    {
        XmlQuery lfm = ws::parse(r);        
        foreach (XmlQuery e, lfm.children( "artist" ))
        {
            // convert floating percentage to int in range 0 to 10,000
            int const match = e["match"].text().toFloat() * 100;
            artists.insertMulti( match, e["name"].text() );
        }
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }
    return artists;
}


static inline QList<QUrl> images( const lastfm::XmlQuery& e )
{
    QList<QUrl> images;
    images += e["image size=small"].text();
    images += e["image size=medium"].text();
    images += e["image size=large"].text();
    return images;
}


QList<Artist> /* static */
Artist::list( QNetworkReply* r )
{
    QList<Artist> artists;
    try {
        XmlQuery lfm = ws::parse(r);
        foreach (XmlQuery xq, lfm.children( "artist" )) {
            Artist artist = xq["name"].text();
            artist.m_images = images( xq );
            artists += artist;
        }
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }
    return artists;
}


Artist
Artist::getInfo( QNetworkReply* r )
{
    try {
        XmlQuery lfm = ws::parse(r);
        Artist artist = lfm["artist"]["name"].text();
        artist.m_images = images( lfm["artist"] );
        return artist;
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
        return Artist();
    }
}


QNetworkReply*
Artist::addTags( const QStringList& tags ) const
{
    if (tags.isEmpty())
        return 0;
    QMap<QString, QString> map = params("addTags");
    map["tags"] = tags.join( QChar(',') );
    return ws::post(map);
}
