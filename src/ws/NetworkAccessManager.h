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

#ifndef LASTFM_WS_ACCESS_MANAGER_H
#define LASTFM_WS_ACCESS_MANAGER_H

#include <lastfm/global.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkProxy>


namespace lastfm {

/** Sets useragent and proxy. Auto detecting the proxy where possible. */
class LASTFM_DLLEXPORT NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

#ifdef Q_WS_WIN
    class Pac *m_pac;
    class InternetConnectionMonitor* m_monitor;
#endif

public:
    NetworkAccessManager( QObject *parent = 0 );
    ~NetworkAccessManager();

    /** PAC allows different proxy configurations depending on the request
      * URL and even UserAgent! Thus we allow you to pass that in, we
      * automatically configure the proxy for every request through 
      * WsAccessManager */
    QNetworkProxy proxy( const QNetworkRequest& = QNetworkRequest() );

protected:
    virtual QNetworkReply* createRequest( Operation, const QNetworkRequest&, QIODevice* outgoingdata = 0 );

private slots:
    void onConnectivityChanged( bool );

private:
    /** this function calls QNetworkAccessManager::setProxy, and thus 
      * configures the proxy correctly for the next request created by
      * createRequest. This is necessary due */
    void applyProxy( const QNetworkRequest& );
};

} //namespace lastfm

#endif
