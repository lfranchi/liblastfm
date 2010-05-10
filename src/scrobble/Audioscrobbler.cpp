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


namespace lastfm
{
    struct AudioscrobblerPrivate
    {
        AudioscrobblerPrivate(const QString& id)
                : id( id )
                , cache( ws::Username )
        {}
        
        ~AudioscrobblerPrivate()
        {
        }

        const QString id;
        ScrobbleCache cache;
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
    connect( lastfm::User::updateNowPlaying( track ), SIGNAL(finished()), SLOT(onNowPlayingReturn()));
}


void
lastfm::Audioscrobbler::cache( const Track& track )
{
    QList<Track> tracks;
    tracks.append( track );
    d->cache.add( tracks );
}


void
lastfm::Audioscrobbler::cache( const QList<Track>& tracks )
{
    qDebug() << tracks;
    d->cache.add( tracks );
}


void
lastfm::Audioscrobbler::submit()
{
    if (d->cache.tracks().isEmpty())
        return;

    if (d->cache.tracks().count() == 1)
    {
        connect( d->cache.tracks()[0].scrobble(), SIGNAL(finished()), SLOT(onSubmissionReturn()));
    }
    else
    {
        // sort in chronological order
        qSort( d->cache.tracks().begin(), d->cache.tracks().end() );
        connect(lastfm::Track::scrobbleBatch( d->cache.tracks().mid(0, 50) ), SIGNAL(finished()), SLOT(onSubmissionReturn()));
    }

    emit status( Scrobbling );
}


void
lastfm::Audioscrobbler::onError( Audioscrobbler::Error code )
{
    qDebug() << code; //TODO error text
}


void
lastfm::Audioscrobbler::onNowPlayingReturn()
{
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
    qDebug() << data;
}


void
lastfm::Audioscrobbler::onSubmissionReturn()
{
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
    qDebug() << data;
}
