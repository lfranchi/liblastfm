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
#include "Handshake.h"
#include "../core/misc.h"
#include "../ws/ws.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QNetworkAccessManager>


ScrobblerHandshake::ScrobblerHandshake( const QString& clientId )
                  : m_clientId( clientId )
{
    request();
}


void
ScrobblerHandshake::request()
{
    if (isActive()) return;
    
    QString timestamp = QString::number( QDateTime::currentDateTime().toTime_t() );
    QString auth_token = lastfm::md5( (lastfm::ws::SharedSecret + timestamp).toUtf8() );

    QString query_string = QString() +
        "?hs=true" +
        "&p=1.2.1"
        "&c=" + m_clientId +
        "&v=" + qApp->applicationVersion() +
        "&u=" + QString(QUrl::toPercentEncoding( lastfm::ws::Username )) +
        "&t=" + timestamp +
        "&a=" + auth_token +
        "&api_key=" + lastfm::ws::ApiKey +
        "&sk=" + lastfm::ws::SessionKey;

    QUrl url = "http://post.audioscrobbler.com:80/" + query_string;
    rp = lastfm::nam()->get( QNetworkRequest(url) );
    connect( rp, SIGNAL(finished()), SLOT(onRequestFinished()) );

    qDebug() << "HTTP GET" << url;
}
