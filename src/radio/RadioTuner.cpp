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

#include "RadioTuner.h"
#include "../core/XmlQuery.h"
#include "../types/Xspf.h"
#include "../ws/ws.h"
using namespace lastfm;

//TODO skips left
//TODO multiple locations for the same track
//TODO set rtp flag in getPlaylist (whether user is scrobbling this radio session or not)


RadioTuner::RadioTuner( const RadioStation& station )
     : m_retry_counter( 0 )
{
    QMap<QString, QString> map;
    map["method"] = "radio.tune";
    map["station"] = station.url();
    QNetworkReply* reply = ws::post(map);
    connect( reply, SIGNAL(finished()), SLOT(onTuneReturn()) );
}


void
RadioTuner::onTuneReturn()
{
    try {
        XmlQuery lfm = ws::parse( (QNetworkReply*)sender() );
        emit title( lfm["station"]["name"].text() );
        fetchFiveMoreTracks();
    }
    catch (ws::ParseError& e)
    {
        emit error( e.enumValue() );
    }
}


bool
RadioTuner::fetchFiveMoreTracks()
{
    //TODO check documentation, I figure this needs a session key
    QMap<QString, QString> map;
    map["method"] = "radio.getPlaylist";
    map["rtp"] = "1"; // see above
    QNetworkReply* reply = ws::post( map );
    connect( reply, SIGNAL(finished()), SLOT(onGetPlaylistReturn()) );
    return true;
}


bool
RadioTuner::tryAgain()
{
    qDebug() << "Bad response count" << m_retry_counter;
    
    if (++m_retry_counter > 5)
        return false;
    fetchFiveMoreTracks();
    return true;
}


void
RadioTuner::onGetPlaylistReturn()
{   
    try {
        XmlQuery lfm = ws::parse( (QNetworkReply*)sender() );
        Xspf xspf( lfm["playlist"] );
        QList<Track> tracks( xspf.tracks() );
        if (tracks.isEmpty()) //often we get empty playlists because php is shit
            throw ws::TryAgainLater;

        m_retry_counter = 0;
        foreach (Track t, tracks)
            MutableTrack( t ).setSource( Track::LastFmRadio );
        m_queue += tracks;
        emit trackAvailable();
    }
    catch (ws::ParseError& e) 
    {
        qWarning() << e.what();

        if (e.enumValue() != ws::TryAgainLater || !tryAgain())
            emit error( e.enumValue() );
    }
}


Track
RadioTuner::takeNextTrack()
{
    //TODO presumably, we should check if fetchMoreTracks is working?
    if (m_queue.isEmpty())
        return Track();
    
    Track result = m_queue.takeFirst();
    if (m_queue.isEmpty())
        fetchFiveMoreTracks();

    return result;
}
