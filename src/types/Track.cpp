/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/
#include "Track.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../core/XmlQuery.h"
#include "../ws/ws.h"
#include <QFileInfo>
#include <QStringList>


lastfm::Track::Track()
{
    d = new TrackData;
    d->null = true;
}


lastfm::Track::Track( const QDomElement& e )
{
    d = new TrackData;

    if (e.isNull()) { d->null = true; return; }
    
    d->artist = e.namedItem( "artist" ).toElement().text();
    d->album =  e.namedItem( "album" ).toElement().text();
    d->title = e.namedItem( "track" ).toElement().text();
    d->trackNumber = 0;
    d->duration = e.namedItem( "duration" ).toElement().text().toInt();
    d->url = e.namedItem( "url" ).toElement().text();
    d->rating = e.namedItem( "rating" ).toElement().text().toUInt();
    d->source = e.namedItem( "source" ).toElement().text().toInt(); //defaults to 0, or lastfm::Track::Unknown
    d->time = QDateTime::fromTime_t( e.namedItem( "timestamp" ).toElement().text().toUInt() );
    
    QDomNodeList nodes = e.namedItem( "extras" ).childNodes();
    for (int i = 0; i < nodes.count(); ++i)
    {
        QDomNode n = nodes.at(i);
        QString key = n.nodeName();
        d->extras[key] = n.toElement().text();
    }
}


QDomElement
lastfm::Track::toDomElement( QDomDocument& xml ) const
{
    QDomElement item = xml.createElement( "track" );
    
    #define makeElement( tagname, getter ) { \
        QString v = getter; \
        if (!v.isEmpty()) \
        { \
            QDomElement e = xml.createElement( tagname ); \
            e.appendChild( xml.createTextNode( v ) ); \
            item.appendChild( e ); \
        } \
    }

    makeElement( "artist", d->artist );
    makeElement( "album", d->album );
    makeElement( "track", d->title );
    makeElement( "duration", QString::number( d->duration ) );
    makeElement( "timestamp", QString::number( d->time.toTime_t() ) );
    makeElement( "url", d->url.toString() );
    makeElement( "source", QString::number( d->source ) );
    makeElement( "rating", QString::number(d->rating) );
    makeElement( "fpId", QString::number(d->fpid) );
    makeElement( "mbId", mbid() );

    QDomElement extras = xml.createElement( "extras" );
    QMapIterator<QString, QString> i( d->extras );
    while (i.hasNext()) {
        QDomElement e = xml.createElement( i.next().key() );
        e.appendChild( xml.createTextNode( i.value() ) );
        extras.appendChild( e );
    }
    item.appendChild( extras );

    return item;
}


QString
lastfm::Track::toString( const QChar& separator ) const
{
    if ( d->artist.isEmpty() )
    {
        if ( d->title.isEmpty() )
            return QFileInfo( d->url.path() ).fileName();
        else
            return d->title;
    }

    if ( d->title.isEmpty() )
        return d->artist;

    return d->artist + ' ' + separator + ' ' + d->title;
}


QString //static
lastfm::Track::durationString( int const duration )
{
    QTime t = QTime().addSecs( duration );
    if (duration < 60*60)
        return t.toString( "m:ss" );
    else
        return t.toString( "hh:mm:ss" );
}


QNetworkReply*
lastfm::Track::share( const User& recipient, const QString& message )
{
    QMap<QString, QString> map = params("share");
    map["recipient"] = recipient;
    if (message.size()) map["message"] = message;
    return ws::post(map);
}


QNetworkReply*
lastfm::MutableTrack::love()
{
    if (d->extras.value("rating").size())
        return 0;
    d->extras["rating"] = "L";
    return ws::post(params("love"));
}


QNetworkReply*
lastfm::MutableTrack::ban()
{
    d->extras["rating"] = "B";
    return ws::post(params("ban"));
}


void
lastfm::MutableTrack::unlove()
{
    QString& r = d->extras["rating"];
    if (r == "L") r = "";
}


QMap<QString, QString>
lastfm::Track::params( const QString& method, bool use_mbid ) const
{
    QMap<QString, QString> map;
    map["method"] = "track."+method;
    if (d->mbid.size() && use_mbid)
        map["mbid"] = d->mbid;
    else {
        map["artist"] = d->artist;
        map["track"] = d->title;
    }
    return map;
}


QNetworkReply*
lastfm::Track::getTopTags() const
{
    return ws::get( params("getTopTags", true) );
}


QNetworkReply*
lastfm::Track::getTags() const
{
    return ws::get( params("getTags", true) );
}


QNetworkReply*
lastfm::Track::addTags( const QStringList& tags ) const
{
    if (tags.isEmpty())
        return 0;
    QMap<QString, QString> map = params("addTags");
    map["tags"] = tags.join( QChar(',') );
    return ws::post(map);
}


QNetworkReply*
lastfm::Track::removeTag( const QString& tag ) const
{
    if (tag.isEmpty())
        return 0;
    QMap<QString, QString> map = params( "removeTag" );
    map["tags"] = tag;
    return ws::post(map);
}


QUrl
lastfm::Track::www() const
{
    return UrlBuilder( "music" ).slash( d->artist ).slash( "_" ).slash( d->title ).url();
}


bool
lastfm::Track::isMp3() const
{
    //FIXME really we should check the file header?
    return d->url.scheme() == "file" &&
           d->url.path().endsWith( ".mp3", Qt::CaseInsensitive );
}


lastfm::Track
lastfm::Track::clone() const
{
    Track copy( *this );
    copy.d.detach();
    return copy;
}
