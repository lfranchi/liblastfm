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

#include "Audioscrobbler.h"
#include "NowPlaying.h"
#include "ScrobbleCache.h"
#include "Handshake.h"
#include "ScrobblerSubmission.h"
#include "../ws/WsKeys.h"

using lastfm::Track;


Audioscrobbler::Audioscrobbler( const QString& clientId )
        : m_clientId( clientId ),
          m_handshake( 0 ), 
          m_np( 0 ), 
          m_submitter( 0 ),
          m_hard_failures( 0 )
{
    m_cache = new ScrobbleCache( Ws::Username );

    handshake();
    submit(); // will submit what's there once the handshake completes
}


Audioscrobbler::~Audioscrobbler()
{
    delete m_cache;
    delete m_handshake;
    delete m_np;
    delete m_submitter;
}


void
Audioscrobbler::handshake() //private
{
    m_hard_failures = 0;

    // if we are here due to hard failure then we need to save what we were 
    // doing and load it back into the new requests
    QByteArray np_data;
    QList<Track> tracks;
    if (m_np) np_data = m_np->postData();
    if (m_submitter) tracks = m_submitter->unsubmittedTracks();

    // we delete to be sure of the state of the QHttp objects, as they are 
    // rather black box
    delete m_handshake;
    delete m_np;
    delete m_submitter;
    
    m_handshake = new ScrobblerHandshake( m_clientId );
    connect( m_handshake, SIGNAL(done( QByteArray )), SLOT(onHandshakeReturn( QByteArray )), Qt::QueuedConnection );
    connect( m_handshake, SIGNAL(responseHeaderReceived( QHttpResponseHeader )), SLOT(onHandshakeHeaderReceived( QHttpResponseHeader )) );
    m_np = new NowPlaying( np_data );
    connect( m_np, SIGNAL(done( QByteArray )), SLOT(onNowPlayingReturn( QByteArray )), Qt::QueuedConnection );
    m_submitter = new ScrobblerSubmission;
    m_submitter->setTracks( tracks );
    connect( m_submitter, SIGNAL(done( QByteArray )), SLOT(onSubmissionReturn( QByteArray )), Qt::QueuedConnection );
}


void
Audioscrobbler::rehandshake() //public
{
    if (!m_submitter->hasSession())
    {
        m_handshake->request();
    }
    else
        // if we still have a valid session, np may have been failing, so just
        // send it as it doesn't hurt
        m_np->request();
}


void
Audioscrobbler::nowPlaying( const Track& track )
{
    m_np->submit( track );
}


void
Audioscrobbler::cache( const Track& track )
{
    m_cache->add( track );
}


void
Audioscrobbler::cache( const QList<Track>& tracks )
{
    m_cache->add( tracks );
}


void
Audioscrobbler::submit()
{
    m_submitter->setTracks( m_cache->tracks() );
    m_submitter->submitNextBatch();
    
    if (m_submitter->isActive()) 
        emit status( Scrobbling );
}


void
Audioscrobbler::onError( Audioscrobbler::Error code )
{
    qDebug() << code; //TODO error text

    switch (code)
    {
        case Audioscrobbler::ErrorBannedClientVersion:
        case Audioscrobbler::ErrorInvalidSessionKey:
        case Audioscrobbler::ErrorBadTime:
            // np and submitter are in invalid state and won't send any requests
            // the app has to tell the user and let them decide what to do
            break;

        default:
            Q_ASSERT( false ); // you (yes you!) have missed an enum value out

        case Audioscrobbler::ErrorThreeHardFailures:
        case Audioscrobbler::ErrorBadSession:
            handshake();
            break;
    }

    emit status( code );
}


#define SPLIT( x ) QList<QByteArray> const results = x.split( '\n' ); QByteArray const code =  results.value( 0 ); qDebug() << x.trimmed();


void
Audioscrobbler::onHandshakeReturn( const QByteArray& result ) //TODO trim before passing here
{
    SPLIT( result )

    if (code == "OK" && results.count() >= 4)
    {
        m_np->setSession( results[1] );
        m_np->setUrl( QString::fromUtf8( results[2] ) );
        m_submitter->setSession( results[1] );
        m_submitter->setUrl( QString::fromUtf8( results[3] ) );

        emit status( Audioscrobbler::Handshaken );

        // submit any queued work
        m_np->request();
        m_submitter->request();
    }
    else if (code == "BANNED")
    {
        onError( Audioscrobbler::ErrorBannedClientVersion );
    }
    else if (code == "BADAUTH")
    {
        onError( Audioscrobbler::ErrorInvalidSessionKey );
    }
    else if (code == "BADTIME")
    {
        onError( Audioscrobbler::ErrorBadTime );
    }
    else
        m_handshake->retry(); //TODO increasing time up to 2 hours
}


void
Audioscrobbler::onNowPlayingReturn( const QByteArray& result )
{
    SPLIT( result )

    if (code == "OK")
    {
        m_np->reset();
    }
    else if (code == "BADSESSION")
    {
        if (!m_submitter->isActive())
        {
            // if scrobbling is happening then there is no way I'm causing
            // duplicate scrobbles! We'll fail next time we try to contact 
            // Last.fm instead
            onError( Audioscrobbler::ErrorBadSession );
        }
    }
    // yep, no else. The protocol says hard fail, I say, don't:
    //  1) if only np is down, then hard failing will just mean a lot of work for the handshake php script with no good reason
    //  2) if both are down, subs will hard fail too, so just rely on that
    //  3) if np is up and subs is down, successful np requests will reset the failure count and possibly prevent timely scrobbles

    // TODO retry if server replies with busy, at least
    // TODO you need a lot more error handling for the scrobblerHttp returns "" case
}


void
Audioscrobbler::onSubmissionReturn( const QByteArray& result )
{
    SPLIT( result )

    if (code == "OK")
    {
        m_hard_failures = 0;
        m_cache->remove( m_submitter->batch() );
        m_submitter->submitNextBatch();

        if (m_submitter->batch().isEmpty())
        {
            emit status( Audioscrobbler::TracksScrobbled );
        }
    }
    else if (code == "BADSESSION")
    {
        onError( Audioscrobbler::ErrorBadSession );
    }
	else if (code.startsWith( "FAILED Plugin bug" ))
	{
		qWarning() << "YOU SUCK! Attempting reasonable error handling...";
		m_cache->remove( m_submitter->batch() );
	}
	else if (++m_hard_failures >= 3)
    {
        onError( Audioscrobbler::ErrorThreeHardFailures );
    }
    else
        m_submitter->retry();
}
