#ifndef NETWORK_CONNECTION_MONITOR_H
#define NETWORK_CONNECTION_MONITOR_H

#include <lastfm/global.h>
#include <QObject>

class LASTFM_DLLEXPORT NetworkConnectionMonitor : public QObject
{
    Q_OBJECT
public:
    NetworkConnectionMonitor( QObject *parent = 0 );
    ~NetworkConnectionMonitor();
    bool isConnected() const;

signals:
    void networkUp();
    void networkDown();

protected:
    void setConnected( bool connected );

private:
    bool m_connected;
};

#endif // NETWORK_CONNECTION_MONITOR_H
