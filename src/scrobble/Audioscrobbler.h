/***************************************************************************
 *   Copyright 2007-2009 Last.fm Ltd.                                      *
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

#ifndef LASTFM_AUDIOSCROBBLER_H
#define LASTFM_AUDIOSCROBBLER_H

#include <lastfm/global.h>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QObject>
#include <QVariant>

namespace lastfm
{
    /** @author Max Howell <max@last.fm>
      * An implementation of the Audioscrobbler Realtime Submissions Protocol 
      * version 1.2.1 for a single Last.fm user
      * http://www.audioscrobbler.net/development/protocol/
      */
    class LASTFM_DLLEXPORT Audioscrobbler : public QObject
    {
        Q_OBJECT

    public:
        /** You will need to do QCoreApplication::setVersion and 
          * QCoreApplication::setApplicationName for this to work, also you will 
          * need to have set all the keys in the Ws namespace in WsKeys.h */
        Audioscrobbler( const QString& clientId );
        ~Audioscrobbler();

    public slots:
        /** will ask Last.fm to update the now playing information for the 
          * authenticated user */
        void nowPlaying( const Track& );
        /** will cache the track, but we won't submit it until you call submit() */
        void cache( const Track& );
        /** will submit the submission cache for this user */
        void submit();

    public:
        void cache( const QList<Track>& );

        /** provided the current session is invalid, we will rehandshake.
          * if the current session is valid, we do nothing. Basically, I don't want
          * to write the code to safely delete currently executing submission
          * requests */
        void rehandshake();
    
    public:
        enum Status
        {
            Connecting,
            Handshaken,
            Scrobbling,
            TracksScrobbled,
        
            StatusMax
        };

        enum Error
        {
            /** the following will show via the status signal, the scrobbler will
              * not submit this session (np too), however caching will continue */
            ErrorBadSession = StatusMax,
            ErrorBannedClientVersion,
            ErrorInvalidSessionKey,
            ErrorBadTime,
            ErrorThreeHardFailures
        };

    signals:
        /** the controller should show status in an appropriate manner */
        void status( int code );

    private slots:
        void onHandshakeReturn( const QByteArray& );
        void onNowPlayingReturn( const QByteArray& );
        void onSubmissionReturn( const QByteArray& );

    private:
        void handshake();
        void onError( Error );

    private:
        class AudioscrobblerPrivate* d;
    };
}


static inline QDebug operator<<( QDebug d, lastfm::Audioscrobbler::Status status )
{
    return d << lastfm::qMetaEnumString<lastfm::Audioscrobbler>( status, "Status" );
}
static inline QDebug operator<<( QDebug d, lastfm::Audioscrobbler::Error error )
{
    return d << lastfm::qMetaEnumString<lastfm::Audioscrobbler>( error, "Status" );
}

#endif
