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

#include "ScrobblerHttp.h"
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../ws/ws.h"


ScrobblerHttp::ScrobblerHttp( QObject* parent )
             : QObject( parent )
{
    m_retry_timer = new QTimer( this );
    m_retry_timer->setSingleShot( true );
    connect( m_retry_timer, SIGNAL(timeout()), SLOT(request()) );
    resetRetryTimer();
}


void
ScrobblerHttp::onRequestFinished()
{    
    if (rp->error() == QNetworkReply::OperationCanceledError)
        ; //we aborted it
    if (rp->error())
    {
        qWarning() << "ERROR!" << rp->error();
        emit done( QByteArray() );
    }
    else
    {
        emit done( rp->readAll() );

        // if it is running then someone called retry() in the slot connected to 
        // the done() signal above, so don't reset it, init
        if (!m_retry_timer->isActive())
            resetRetryTimer();
    }
    
    rp->deleteLater();
}


void
ScrobblerHttp::retry()
{
    if (!m_retry_timer->isActive())
    {
        int const i = m_retry_timer->interval();
        if (i < 120 * 60 * 1000)
            m_retry_timer->setInterval( i * 2 );
    }

    qDebug() << "Will retry in" << m_retry_timer->interval() / 1000 << "seconds";

    m_retry_timer->start();
}


void
ScrobblerHttp::resetRetryTimer()
{
    m_retry_timer->setInterval( 30 * 1000 );
}


void
ScrobblerPostHttp::request()
{
    if (m_data.isEmpty() || m_session.isEmpty())
        return;

    if (rp) 
        rp->deleteLater();
    
    QByteArray data = "s=" + m_session + m_data;

    QNetworkRequest rq( m_url );
    rq.setRawHeader( "Content-Type", "application/x-www-form-urlencoded" );
    rp = lastfm::nam()->post( rq, data );
    connect( rp, SIGNAL(finished()), SLOT(onRequestFinished()) );
    rp->setParent( this );

    qDebug() << "HTTP POST:" << m_url << data;
}
