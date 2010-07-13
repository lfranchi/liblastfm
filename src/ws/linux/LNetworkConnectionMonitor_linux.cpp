#include "LNetworkConnectionMonitor.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

LNetworkConnectionMonitor::LNetworkConnectionMonitor( QObject* parent ) :
    NetworkConnectionMonitor( parent )
{
    m_nmInterface = new QDBusInterface( QString( "org.freedesktop.NetworkManager" ),
                                        QString( "/org/freedesktop/NetworkManager" ),
                                        QString( "org.freedesktop.NetworkManager" ),
                                        QDBusConnection::systemBus(),
                                        this );

    //get current connection state
    QDBusInterface* dbusInterface = new QDBusInterface( QString( "org.freedesktop.NetworkManager" ),
                                                        QString( "/org/freedesktop/NetworkManager" ),
                                                        QString( "org.freedesktop.DBus.Properties" ),
                                                        QDBusConnection::systemBus(),
                                                        this );

    QDBusReply<QVariant> reply = dbusInterface->call( "Get", "org.freedesktop.NetworkManager", "state" );
    if ( reply.isValid() )
    {
        if ( reply.value() == Connected )
        {
            setConnected( true );
        }
        else if ( reply.value() == Disconnected )
        {
            setConnected( false );
        }
    }
    else
    {
        qDebug() << "Error: " << reply.error();
    }
    delete dbusInterface;

    //connect network manager signals
   connect( m_nmInterface, SIGNAL( StateChange( uint ) ), this, SLOT( onStateChange( uint ) ) );

}

LNetworkConnectionMonitor::~LNetworkConnectionMonitor()
{
    delete m_nmInterface;
}


void
LNetworkConnectionMonitor::onStateChange( uint newState )
{
    qDebug() << "Networkmanager state change!";
    
    if ( newState == Disconnected )
    {
       setConnected( false );
    }
    else if ( newState == Connected )
    {
       setConnected( true );
    }
}
