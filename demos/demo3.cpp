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

#include <lastfm.h>
#include <QtCore>
#include <QtNetwork>
#include "src/_version.h"


struct MyCoreApp : QCoreApplication
{
    Q_OBJECT

public:
    MyCoreApp( int& argc, char** argv ) : QCoreApplication( argc, argv )
    {}

public slots:
    void onStatus( int status )
    {
        qDebug() << lastfm::Audioscrobbler::Status(status);
    }
};


int main( int argc, char** argv )
{
    // all 6 of these lines are REQUIRED in order to scrobble
    // this demo requires you to fill in the blanks as well...
    lastfm::ws::Username = 
    lastfm::ws::ApiKey =
    lastfm::ws::SharedSecret =
    lastfm::ws::SessionKey = // you need to auth to get this... try demo2
    QCoreApplication::setApplicationName( "liblastfm" );
    QCoreApplication::setApplicationVersion( VERSION );

    MyCoreApp app( argc, argv );
    
    lastfm::MutableTrack t;
    t.setArtist( "Max Howell" );
    t.setTitle( "I Told You Not To Trust Me With Your Daughter" );
    t.setDuration( 30 );
    t.stamp(); //sets track start time
    
    lastfm::Audioscrobbler as( "ass" );
    as.nowPlaying( t );
    // Audioscrobbler will submit whatever is in the cache when you call submit.
    // And the cache is persistent between sessions. So you should cache at the
    // scrobble point usually, not before
    as.cache( t );
    
    //FIXME I don't get it, but the timer never triggers! pls fork and fix!
    QTimer::singleShot( 31*1000, &as, SLOT(submit()) );
    
    app.connect( &as, SIGNAL(status(int)), SLOT(onStatus(int)) );
    
    return app.exec();
}


#include "demo3.moc"
