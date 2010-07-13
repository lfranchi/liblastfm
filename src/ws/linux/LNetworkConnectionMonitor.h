#ifndef LNETWORK_CONNECTION_MONITOR_H
#define LNETWORK_CONNECTION_MONITOR_H

#include "../NetworkConnectionMonitor.h"
#include <lastfm/global.h>
#include <QObject>

class QDBusConnection;
class QDBusInterface;

class LNetworkConnectionMonitor : public NetworkConnectionMonitor
{
    Q_OBJECT

    enum NMState
    {
        Unknown=1,
        Asleep,
        Connected,
        Disconnected
    };

public:
    LNetworkConnectionMonitor( QObject* parent = 0 );
    ~LNetworkConnectionMonitor();
private slots:
    void onStateChange( uint newState );
private:
    QDBusInterface* m_nmInterface;
};

#endif // LNETWORK_CONNECTION_MONITOR_H

