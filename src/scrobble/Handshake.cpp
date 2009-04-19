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

#include "Handshake.h"
#include "common/qt/md5.cpp"
#include "../ws/WsKeys.h"
#include "../ws/WsRequestBuilder.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
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
    QString auth_token = Qt::md5( (Ws::SharedSecret + timestamp).toUtf8() );

    QString query_string = QString() +
        "?hs=true" +
        "&p=1.2.1"
        "&c=" + m_clientId +
        "&v=" + qApp->applicationVersion() +
        "&u=" + QString(QUrl::toPercentEncoding( Ws::Username )) +
        "&t=" + timestamp +
        "&a=" + auth_token +
        "&api_key=" + Ws::ApiKey +
        "&sk=" + Ws::SessionKey;

    QUrl url = "http://post.audioscrobbler.com:80/" + query_string;
    rp = WsRequestBuilder::nam()->get( QNetworkRequest(url) );
    connect( rp, SIGNAL(finished()), SLOT(onRequestFinished()) );

    qDebug() << "HTTP GET" << url;
}
