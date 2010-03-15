/*
   Copyright 2009 Last.fm Ltd. 
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
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../core/XmlQuery.h"
#include <QStringList>
using lastfm::User;
using lastfm::UserList;
using lastfm::UserDetails;

QMap<QString, QString>
User::params(const QString& method) const
{
    QMap<QString, QString> map;
    map["method"] = "user."+method;
    map["user"] = m_name;
    return map;
}


QNetworkReply*
User::getFriends( int perPage, int page ) const
{
    QMap<QString, QString> map = params( "getFriends" );
    map["limit"] = QString::number(perPage);
    map["page"] = QString::number(page);
    return ws::get( map );
}


QNetworkReply*
User::getTopTags() const
{
    return ws::get( params( "getTopTags" ) );
}


QNetworkReply*
User::getTopArtists() const
{
    return ws::get( params( "getTopArtists" ) );
}


QNetworkReply*
User::getRecentArtists() const
{
    return ws::get( params( "getRecentArtists" ) );
}


QNetworkReply*
User::getRecentTracks() const
{
    return ws::get( params( "getRecentTracks" ) );
}

QNetworkReply* 
User::getRecentStations() const
{
    return ws::post( params( "getRecentStations" ) );
}

QNetworkReply*
User::getNeighbours() const
{
    return ws::get( params( "getNeighbours" ) );
}


QNetworkReply*
User::getPlaylists() const
{
    return ws::get( params( "getPlaylists" ) );
}


UserList //static
User::list( QNetworkReply* r )
{
    UserList users;
    try {
        XmlQuery lfm = ws::parse(r);
        foreach (XmlQuery e, lfm.children( "user" ))
        {
            User u( e["name"].text() );
            u.m_smallImage = e["image size=small"].text();
            u.m_mediumImage = e["image size=medium"].text();
            u.m_largeImage = e["image size=large"].text();
            u.m_realName = e["realname"].text();
            users += u;
        }

        users.total = lfm["friends"].attribute("total").toInt();
        users.page = lfm["friends"].attribute("page").toInt();
        users.perPage = lfm["friends"].attribute("perPage").toInt();
        users.totalPages = lfm["friends"].attribute("totalPages").toInt();
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }    
    return users;
}


QNetworkReply* //static
UserDetails::getInfo( const QString& username )
{
    QMap<QString, QString> map;
    map["method"] = "user.getInfo";
    map["user"] = username;
    return ws::post( map );
}


/*
QNetworkReply* //static
UserDetails::getRecommendedArtists()
{
    QMap<QString, QString> map;
    map["method"] = "user.getRecommendedArtists";
    return ws::post( map );
}
*/

QUrl
User::www() const
{ 
    return UrlBuilder( "user" ).slash( m_name ).url();
}


UserDetails::UserDetails( QNetworkReply* reply )
{
    try
    {
        XmlQuery user = XmlQuery(ws::parse(reply))["user"];
        m_age = user["age"].text().toUInt();
        m_scrobbles = user["playcount"].text().toUInt();
        m_registered = QDateTime::fromTime_t(user["registered"].attribute("unixtime").toUInt());
        m_country = user["country"].text();
        m_isSubscriber = ( user["subscriber"].text() == "1" );
        m_canBootstrap = ( user["bootstrap"].text() == "1" );
        m_gender = ( user["gender"].text());
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }
}


QString
UserDetails::getInfoString() const
{
    #define tr QObject::tr
;

    QString text;
    if (m_gender.known() && m_age > 0 && m_scrobbles > 0)
    {
        text = tr("A %1, %2 years of age with %L3 scrobbles")
                .arg( m_gender.toString() )
                .arg( m_age )
                .arg( m_scrobbles );
    }
    else if (m_scrobbles > 0)
    {
        text = tr("%L1 scrobbles").arg( m_scrobbles );
    }    

    return text;
    
    #undef tr
}
