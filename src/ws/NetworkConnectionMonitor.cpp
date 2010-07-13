#include "NetworkConnectionMonitor.h"

NetworkConnectionMonitor::NetworkConnectionMonitor( QObject* parent )
    : m_connected( true )
{
}

NetworkConnectionMonitor::~NetworkConnectionMonitor()
{
}

bool
NetworkConnectionMonitor::isConnected() const
{
    return m_connected;
}

void
NetworkConnectionMonitor::setConnected( bool connected )
{
    if ( m_connected != connected )
    {
        m_connected = connected;

        if ( connected )
            emit networkUp();
        else
            emit networkDown();
    }
}

