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
#include "../ws/ws.h"


namespace lastfm
{
    struct AudioscrobblerPrivate
    {
        AudioscrobblerPrivate(const QString& id)
                : id( id )
                , cache( ws::Username )
                , hard_failures( 0 )
        {}
        
        ~AudioscrobblerPrivate()
        {
            delete handshake;
            delete np;
            delete submitter;
        }

        const QString id;
        QPointer<ScrobblerHandshake> handshake;
        QPointer<NowPlaying> np;
        QPointer<ScrobblerSubmission> submitter;
        ScrobbleCache cache;
        uint hard_failures;
    };
}


lastfm::Audioscrobbler::Audioscrobbler( const QString& id )
        : d( new AudioscrobblerPrivate(id) )
{
    handshake();
    submit(); // will submit what's there once the handshake completes
}


lastfm::Audioscrobbler::~Audioscrobbler()
{
    delete d;
}


void
lastfm::Audioscrobbler::handshake() //private
{
    d->hard_failures = 0;

    // if we are here due to hard failure then we need to save what we were 
    // doing and load it back into the new requests
    QByteArray np_data;
    QList<Track> tracks;
    if (d->np) np_data = d->np->postData();
    if (d->submitter) tracks = d->submitter->unsubmittedTracks();

    // we delete to be sure of the state of the QHttp objects, as they are 
    // rather black box
    delete d->handshake;
    delete d->np;
    delete d->submitter;
    
    d->handshake = new ScrobblerHandshake( d->id );
    connect( d->handshake, SIGNAL(done( QByteArray )), SLOT(onHandshakeReturn( QByteArray )), Qt::QueuedConnection );
    d->np = new NowPlaying( np_data );
    connect( d->np, SIGNAL(done( QByteArray )), SLOT(onNowPlayingReturn( QByteArray )), Qt::QueuedConnection );
    d->submitter = new ScrobblerSubmission;
    d->submitter->setTracks( tracks );
    connect( d->submitter, SIGNAL(done( QByteArray )), SLOT(onSubmissionReturn( QByteArray )), Qt::QueuedConnection );
}


void
lastfm::Audioscrobbler::rehandshake() //public
{
    if (!d->submitter->hasSession())
    {
        d->handshake->request();
    }
    else
        // if we still have a valid session, np may have been failing, so just
        // send it as it doesn't hurt
        d->np->request();
}


void
lastfm::Audioscrobbler::nowPlaying( const Track& track )
{
    d->np->submit( track );
}


void
lastfm::Audioscrobbler::cache( const Track& track )
{
    d->cache.add( track );
}


void
lastfm::Audioscrobbler::cache( const QList<Track>& tracks )
{
    d->cache.add( tracks );
}


void
lastfm::Audioscrobbler::submit()
{
    d->submitter->setTracks( d->cache.tracks() );
    d->submitter->submitNextBatch();
    
    if (d->submitter->isActive()) 
        emit status( Scrobbling );
}


void
lastfm::Audioscrobbler::onError( Audioscrobbler::Error code )
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
lastfm::Audioscrobbler::onHandshakeReturn( const QByteArray& result ) //TODO trim before passing here
{
    SPLIT( result )

    if (code == "OK" && results.count() >= 4)
    {
        d->np->setSession( results[1] );
        d->np->setUrl( QString::fromUtf8( results[2] ) );
        d->submitter->setSession( results[1] );
        d->submitter->setUrl( QString::fromUtf8( results[3] ) );

        emit status( Audioscrobbler::Handshaken );

        // submit any queued work
        d->np->request();
        d->submitter->request();
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
        d->handshake->retry(); //TODO increasing time up to 2 hours
}


void
lastfm::Audioscrobbler::onNowPlayingReturn( const QByteArray& result )
{
    SPLIT( result )

    if (code == "OK")
    {
        d->np->reset();
    }
    else if (code == "BADSESSION")
    {
        // don't do anything, because we should use hard-failure route to ensure
        // we don't do an infinitely fast np:BADSESSION->handshake() loop
        // but we don't so we can't do anything
    }
    // yep, no else. The protocol says hard fail, I say, don't:
    //  1) if only np is down, then hard failing will just mean a lot of work for the handshake php script with no good reason
    //  2) if both are down, subs will hard fail too, so just rely on that
    //  3) if np is up and subs is down, successful np requests will reset the failure count and possibly prevent timely scrobbles

    // TODO retry if server replies with busy, at least
    // TODO you need a lot more error handling for the scrobblerHttp returns "" case
}


void
lastfm::Audioscrobbler::onSubmissionReturn( const QByteArray& result )
{
    SPLIT( result )

    if (code == "OK")
    {
        d->hard_failures = 0;
        d->cache.remove( d->submitter->batch() );
        d->submitter->submitNextBatch();

        if (d->submitter->batch().isEmpty())
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
        d->cache.remove( d->submitter->batch() );
    }
    else if (++d->hard_failures >= 3)
    {
        onError( Audioscrobbler::ErrorThreeHardFailures );
    }
    else
        d->submitter->retry();
}
