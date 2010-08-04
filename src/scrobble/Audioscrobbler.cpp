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

#include <QPointer>

#include "Audioscrobbler.h"
#include "ScrobbleCache.h"

#include "../types/User.h"
#include "../ws/ws.h"
#include "../core/XmlQuery.h"


namespace lastfm
{
    struct AudioscrobblerPrivate
    {
        AudioscrobblerPrivate(const QString& id)
                : m_id( id )
                , m_cache( ws::Username )
        {}
        
        ~AudioscrobblerPrivate()
        {
        }

        const QString m_id;
        ScrobbleCache m_cache;
        QList<Track> m_batch;
        QPointer<QNetworkReply> m_nowPlayingReply;
        QPointer<QNetworkReply> m_scrobbleReply;
    };
}


lastfm::Audioscrobbler::Audioscrobbler( const QString& id )
        : d( new AudioscrobblerPrivate(id) )
{
    submit();
}


lastfm::Audioscrobbler::~Audioscrobbler()
{
    delete d;
}


void
lastfm::Audioscrobbler::nowPlaying( const Track& track )
{
    if ( d->m_nowPlayingReply.isNull())
    {
        d->m_nowPlayingReply = lastfm::User::updateNowPlaying( track );
        connect( d->m_nowPlayingReply, SIGNAL(finished()), SLOT(onNowPlayingReturn()));
    }
}


void
lastfm::Audioscrobbler::cache( const Track& track )
{
    QList<Track> tracks;
    tracks.append( track );
    cache( tracks );
}


void
lastfm::Audioscrobbler::cache( const QList<Track>& tracks )
{
    d->m_cache.add( tracks );

    foreach ( const Track& track, d->m_cache.tracks() )
        MutableTrack( track ).setScrobbleStatus( Track::Cached );

    emit scrobblesCached( tracks );
    submit();
}


void
lastfm::Audioscrobbler::submit()
{
    if (d->m_cache.tracks().isEmpty() // there are no tracks to submit
            || !d->m_scrobbleReply.isNull() ) // we are already submitting scrobbles
        return;

    // copy tracks to be submitted to a temporary list
    d->m_batch = d->m_cache.tracks().mid( 0, 50 );

    // if there is only one track use track.scrobble, otherwise use track.scrobbleBatch
    if (d->m_batch.count() == 1)
    {
        d->m_scrobbleReply = d->m_batch[0].scrobble();
        connect( d->m_scrobbleReply, SIGNAL(finished()), SLOT(onTrackScrobbleReturn()));
    }
    else
    {
        d->m_scrobbleReply = lastfm::Track::scrobbleBatch( d->m_batch );
        connect( d->m_scrobbleReply, SIGNAL(finished()), SLOT(onTrackScrobbleBatchReturn()));
    }
}


void
lastfm::Audioscrobbler::onNowPlayingReturn()
{
    lastfm::XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();
    qDebug() << lfm;

    if ( lfm.attribute("status") != "ok" )
    {
        emit nowPlayingError( lfm["code"].text().toInt(), lfm["error"].text() );
    }

    d->m_nowPlayingReply = 0;
}


void
lastfm::Audioscrobbler::onTrackScrobbleReturn()
{
    Q_ASSERT(d->m_batch.count() == 1, "Audioscrobbler", "Not 1 track in batch after track.Scrobble");

    lastfm::XmlQuery lfm = d->m_scrobbleReply->readAll();
    qDebug() << lfm;

    if (d->m_scrobbleReply->error() == QNetworkReply::NoError)
    {
        if (lfm.attribute("status") == "ok")
        {
            MutableTrack( d->m_batch.at(0) ).setScrobbleStatus( Track::Submitted );
        }
        else
        {
            qDebug() << lfm["error"].attribute("code");
            MutableTrack track = MutableTrack( d->m_batch.at(0) );
            track.setScrobbleError( static_cast<Track::ScrobbleError>(lfm["error"].attribute("code").toInt()) );
            track.setScrobbleStatus( Track::Error );
        }

        d->m_cache.remove( d->m_batch );
        d->m_batch.clear();
    }
    else
    {
        if ( d->m_scrobbleReply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt() == 400)
        {
            d->m_cache.remove( d->m_batch );
            d->m_batch.clear();
        }
    }

    d->m_scrobbleReply = 0;
}

void
lastfm::Audioscrobbler::onTrackScrobbleBatchReturn()
{
    lastfm::XmlQuery lfm = d->m_scrobbleReply->readAll();
    qDebug() << lfm;

    if (d->m_scrobbleReply->error() == QNetworkReply::NoError)
    {
        if (lfm.attribute("status") == "ok")
        {
            foreach( const Track& track, d->m_batch )
                MutableTrack( track ).setScrobbleStatus( Track::Submitted );
        }
        else
        {
            qDebug() << lfm["error"].attribute("code");
            foreach( const Track& track, d->m_batch )
            {
                MutableTrack mutableTrack( track );
                mutableTrack.setScrobbleError( static_cast<Track::ScrobbleError>(lfm["error"].attribute("code").toInt()) );
                mutableTrack.setScrobbleStatus( Track::Error );
            }
        }

        d->m_cache.remove( d->m_batch );
        d->m_batch.clear();
    }
    else
    {
        if ( d->m_scrobbleReply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt() == 400)
        {
            d->m_cache.remove( d->m_batch );
            d->m_batch.clear();
        }
    }

    d->m_scrobbleReply = 0;
}
