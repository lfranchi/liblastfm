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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef LASTFM_CONNECTION_MONITOR_H
#define LASTFM_CONNECTION_MONITOR_H

#include <lastfm/global.h>
#include <QObject>
#ifdef Q_WS_MAC
#include <SystemConfiguration/SCNetwork.h> //TODO remove
typedef const struct __SCNetworkReachability * SCNetworkReachabilityRef;
#endif


namespace lastfm {

class LASTFM_DLLEXPORT InternetConnectionMonitor : public QObject
{
    Q_OBJECT

#ifdef Q_WS_MAC
    static void callback( SCNetworkReachabilityRef, SCNetworkConnectionFlags, void* );
#endif
#ifdef Q_WS_WIN
    class NdisEventsProxy* m_ndisEventsProxy;
    friend class NdisEventsProxy;
#endif

    bool m_up;
    
public:
    /** if internet is unavailable you will get a down() signal soon, otherwise
      * you won't get a signal until the net goes down */
    InternetConnectionMonitor( QObject *parent = 0 );

    bool isDown() const { return !m_up; }
    bool isUp() const { return m_up; }
    
signals:
    /** yay! internet has returned */
    void up( const QString& connectionName = "" );
    
    /** we think the internet is unavailable, but well, still try, but show
      * an unhappy face in the statusbar or something */
    void down( const QString& connectionName = "" );
    
    /** emitted after the above */
    void connectivityChanged( bool );
};

} //namespace lastfm

#endif