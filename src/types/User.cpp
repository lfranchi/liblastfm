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
using lastfm::AuthenticatedUser;


QMap<QString, QString>
User::params(const QString& method) const
{
    QMap<QString, QString> map;
    map["method"] = "user."+method;
    map["user"] = m_name;
    return map;
}


QNetworkReply*
User::getFriends() const
{
    return ws::get( params( "getFriends" ) );
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
User::getNeighbours() const
{
    return ws::get( params( "getNeighbours" ) );
}


QNetworkReply*
User::getPlaylists() const
{
    return ws::get( params( "getPlaylists" ) );
}


QList<User> //static
User::list( QNetworkReply* r )
{
    QList<User> users;
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
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }    
    return users;
}


QNetworkReply* //static
AuthenticatedUser::getInfo()
{
    QMap<QString, QString> map;
    map["method"] = "user.getInfo";
    return ws::post( map );
}


QNetworkReply* //static
AuthenticatedUser::getRecommendedArtists()
{
    QMap<QString, QString> map;
    map["method"] = "user.getRecommendedArtists";
    return ws::post( map );
}


QUrl
User::www() const
{ 
    return UrlBuilder( "user" ).slash( m_name ).url();
}


QString //static
AuthenticatedUser::getInfoString( QNetworkReply* reply )
{
    #define tr QObject::tr
    
    class Gender
    {
        QString s;

    public:
        Gender( const QString& ss ) :s( ss.toLower() )
        {}
 
        bool known() const { return male() || female(); }
        bool male() const { return s == "m"; }
        bool female() const { return s == "f"; }
 
        QString toString()
        {
            QStringList list;
            if (male())
                list << tr("boy") << tr("lad") << tr("chap") << tr("guy");
            else if (female())
                // I'm not sexist, it's just I'm gutless and couldn't think
                // of any other non offensive terms for women!
                list << tr("girl") << tr("lady") << tr("lass");
            else 
                return tr("person");
            
            return list.value( QDateTime::currentDateTime().toTime_t() % list.count() );
        }
    };

    QString text;
    try
    {
        XmlQuery user = XmlQuery(ws::parse(reply))["user"];
        Gender gender = user["gender"].text();
        QString age = user["age"].text();
        uint const scrobbles = user["playcount"].text().toUInt();
        if (gender.known() && age.size() && scrobbles > 0)
        {
            text = tr("A %1, %2 years of age with %L3 scrobbles")
                    .arg( gender.toString() )
                    .arg( age )
                    .arg( scrobbles );
        }
        else if (scrobbles > 0)
        {
            text = tr("%L1 scrobbles").arg( scrobbles );
        }    
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }
    return text;
    
    #undef tr
}
