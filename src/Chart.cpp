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
#include <QNetworkReply>

#include "ws.h"

#include "Chart.h"

lastfm::Chart::Chart()
{
}

QNetworkReply*
lastfm::Chart::getHypedArtists( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getHypedArtists";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}

QNetworkReply*
lastfm::Chart::getHypedTracks( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getHypedTracks";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}

QNetworkReply*
lastfm::Chart::getLovedTracks( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getLovedTracks";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}

QNetworkReply*
lastfm::Chart::getTopArtists( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getTopArtists";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}

QNetworkReply*
lastfm::Chart::getTopDownloads( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getTopDownloads";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}

QNetworkReply*
lastfm::Chart::getTopTags( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getTopTags";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}

QNetworkReply*
lastfm::Chart::getTopTracks( int limit, int page )
{
    QMap<QString, QString> map;
    map["method"] = "chart.getTopTracks";
    map["page"] =QString::number( page );
    map["limit"] = QString::number( limit );
    return ws::get( map );
}
