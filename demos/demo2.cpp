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
 
#include "src/ws/WsReplyBlock.h" // it's naughty to use this
#include "common/qt/md5.cpp"     // we made this to ensure DRY
#include <lastfm.h>
#include <QtCore>


struct MyCoreApp : QCoreApplication
{
    Q_OBJECT
    
public:
    MyCoreApp( int& argc, char**& argv ) : QCoreApplication( argc, argv )
    {}
    
private slots:
    void onWsError( Ws::Error e )
    {
        // WsReply will invoke this slot on application level errors
        // mostly this is only stuff like Ws::InvalidSessionKey and
        // Ws::InvalidApiKey    
        qWarning() << e;
    }
};


int main( int argc, char** argv )
{
    MyCoreApp app( argc, argv );
    
////// you'll need to fill these in for this demo to work
    Ws::Username = "2girls1cup";
    Ws::ApiKey = "e57a776f8279e546e2aa00e132e5ae2d";
    Ws::SharedSecret = "be955c47b4390be5992696595c0abc34"; //ssssh!
    QString password = "TESTarse1";

////// Usually you never have to construct an Last.fm WS API call manually
    // eg. Track.getTopTags() just returns a WsReply* but authentication is
    // different.
    // We're using getMobileSession here as we're a console app, but you 
    // should use getToken if you can as the user will be presented with a
    // route that feels my trustworthy to them than entering their password
    // into some random app they just downloaded... ;)
    WsReply* reply = WsRequestBuilder( "auth.getMobileSession" )
            .add( "username", Ws::Username )
            .add( "authToken", Qt::md5( (Ws::Username + Qt::md5( password.toUtf8() )).toUtf8() ) )
            .get();
    
    // never do this when an event loop is running
    WsReplyBlock::wait( reply );
    
    try
    {
    ////// Usually there is a convenience function to decode the output from
        // ws calls too, but again, authentication is different. We think you
        // need to handle it yourselves :P Also conveniently it means you
        // can learn more about what our webservices return, eg. this service
        // will return an XML document like this:
        //
        // <lfm status="ok">
        //   <session>
        //     <name>mxcl</name>
        //      <key>d580d57f32848f5dcf574d1ce18d78b2</key>
        //      <subscriber>1</subscriber>
        //   </session>
        // </lfm>
        //
        // If status is not "ok" then WsReply::error() will return something 
        // useful.
        WsDomElement const session = reply->lfm()["session"];
        
        // replace username; because eg. perhaps the user typed their
        // username with the wrong case
        Ws::Username = session["name"].text();
        
        // we now have a session key, you should save this, forever! Really.
        // DO NOT AUTHENTICATE EVERY TIME THE APP STARTS! You only have to do
        // this once. Or again if the user deletes your key on the site. If 
        // that happens you'll get notification to your onWsError() function,
        // see above.
        Ws::SessionKey = session["key"].nonEmptyText();
        
        qDebug() << "sk:" << Ws::SessionKey;
        
    ////// because the Ws::SessionKey is now set, the AuthenticatedUser
        // class will work. And we can call authenticated calls
        WsReply* reply = lastfm::AuthenticatedUser().getRecommendedArtists();

        // again, you shouldn't use these.. ;)
        WsReplyBlock::wait( reply );
        
        // yay, a list rec'd artists to stderr :)
        qDebug() << lastfm::Artist::list( reply );
    }
    catch (std::runtime_error& e)
    {
        // some calls throw if the data we receive from Last.fm is malformed
        qWarning() << e.what();
        return 1;
    }
}


#include "demo2.moc"
