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
#include "InternetConnectionMonitor.h"
#include "ws.h"

#ifdef __APPLE__
#include <QPointer>
#include <SystemConfiguration/SCNetworkReachability.h>
QList<QPointer<lastfm::InternetConnectionMonitor> > monitors;
#endif


#ifdef WIN32

// WsAccessManager needs special init (on Windows), and it needs to be done
// early, so be careful about moving this
#include "win/ComSetup.h" //must be first header or compile fail results!
#include "win/NdisEvents.h"
static ComSetup com_setup;

namespace lastfm {

// bounce NdisEvents signals through here so we don't have to expose the 
// NdisEvents interface in InternetConnectionMonitor  :)
class NdisEventsProxy : public NdisEvents
{
public:
    NdisEventsProxy(InternetConnectionMonitor *icm)
        :m_icm(icm)
    {
    }

    // WmiSink callbacks:
    virtual void onConnectionUp( BSTR name )
    {
        emit m_icm->up( QString::fromUtf16(name) );
        emit m_icm->connectivityChanged( true );
    }
    
    virtual void onConnectionDown( BSTR name )
    {
        emit m_icm->down( QString::fromUtf16(name) );
        emit m_icm->connectivityChanged( false );
    }

    InternetConnectionMonitor* m_icm;
};

}

#endif


lastfm::InternetConnectionMonitor::InternetConnectionMonitor( QObject *parent )
                                 : QObject( parent )
                                 , m_up( true )
{
#ifdef __APPLE__
    if (monitors.isEmpty())
    {
        SCNetworkReachabilityRef ref = SCNetworkReachabilityCreateWithName( NULL, LASTFM_WS_HOSTNAME );
        SCNetworkReachabilityScheduleWithRunLoop( ref, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode );
        SCNetworkReachabilitySetCallback( ref, callback, NULL );
        CFRelease( ref );
    }
    
    QPointer<InternetConnectionMonitor> p = this;
    monitors += p;
#endif
#ifdef WIN32
    m_ndisEventsProxy = new NdisEventsProxy(this);
    m_ndisEventsProxy->registerForNdisEvents();
#endif
}


#ifdef __APPLE__
void
lastfm::InternetConnectionMonitor::callback( SCNetworkReachabilityRef, SCNetworkConnectionFlags flags, void* )
{
    static bool up = true;
        
    // I couldn't find any diffinitive usage examples for these flags
    // so I had to guess, since I can't test, eg. dial up :(
    
    bool b;
    if (flags & kSCNetworkFlagsConnectionRequired)
        b = false;
    else
        b = flags & (kSCNetworkFlagsReachable | kSCNetworkFlagsTransientConnection | kSCNetworkFlagsConnectionAutomatic);
    
    qDebug() << "Can reach " LASTFM_WS_HOSTNAME ":" << b << ", flags:" << flags;
    
    // basically, avoids telling everyone that we're up already on startup
    if (up == b) return;
    up = b;
    
    foreach (InternetConnectionMonitor* monitor, monitors)
        if (monitor) 
        {
            monitor->m_up = b;
            
            if (b)
                emit monitor->up();
            else
                emit monitor->down();

            emit monitor->connectivityChanged( b );
        }
}
#endif


