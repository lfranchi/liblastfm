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

#include "Tag.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../core/XmlQuery.h"
#include "../ws/ws.h"
using lastfm::Tag;
using lastfm::User;


QUrl
Tag::www() const
{
    return UrlBuilder( "tag" ).slash( m_name ).url();
}


QUrl
Tag::www( const User& user ) const
{
    return UrlBuilder( "user" ).slash( user.name() ).slash( "tags" ).slash( Tag::name() ).url();
}


QNetworkReply*
Tag::search() const
{
    QMap<QString, QString> map;
    map["method"] = "tag.search";
    map["tag"] = m_name;
    return ws::get(map);
}


QMap<int, QString> //static
Tag::list( QNetworkReply* r )
{
    QMap<int, QString> tags;
    try {
        foreach (XmlQuery xq, XmlQuery(ws::parse(r)).children("tag"))
            // we toLower always as otherwise it is ugly mixed case, as first
            // ever tag decides case, and Last.fm is case insensitive about it 
            // anyway
            tags.insertMulti( xq["count"].text().toInt(), xq["name"].text().toLower() );
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }
    return tags;
}
