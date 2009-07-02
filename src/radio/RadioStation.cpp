/*
   Copyright 2009 Last.fm Ltd. 

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

#include "RadioStation.h"
#include "../core/XmlQuery.h"

//static 
QList<lastfm::RadioStation> 
lastfm::RadioStation::list( QNetworkReply* r )
{
    QList<lastfm::RadioStation> result;
    try {
        foreach (XmlQuery xq, XmlQuery(ws::parse(r)).children("station")) {
            lastfm::RadioStation rs(xq["url"].text());
            rs.setTitle(xq["name"].text());
            result.append(rs);
        }
    }
    catch (ws::ParseError& e)
    {
        qWarning() << e.what();
    }
    return result;
}
