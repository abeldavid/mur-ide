#include "connectionmanager.h"
#include "wificonnection.h"
#include "bluetoothconnection.h"

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent)
{
    connectToWifi();
}

void ConnectionManager::runApp()
{
    m_connection->runApp();
}

void ConnectionManager::killApp()
{
    m_connection->killApp();
}

void ConnectionManager::sendFile(QString file)
{
    m_connection->sendFile(file);
}

void ConnectionManager::connectSignals()
{
    connect(m_connection, SIGNAL(appSent(bool)), this, SIGNAL(appSent(bool)));
    connect(m_connection, SIGNAL(appStarted(bool)), this, SIGNAL(appStarted(bool)));
    connect(m_connection, SIGNAL(appKilled(bool)), this, SIGNAL(appKilled(bool)));
    connect(m_connection, SIGNAL(statusUpdated(StatusInfo)), this, SIGNAL(statusUpdated(StatusInfo)));
    connect(m_connection, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

void ConnectionManager::connectToWifi()
{
    //delete m_connection;
    m_connection = new WiFiConnection(this);
    connectSignals();
}

void ConnectionManager::connectToBluetooth()
{
//    delete m_connection;
//    m_connection = new BluetoothConnection(this);
//    connectSignals();
}
