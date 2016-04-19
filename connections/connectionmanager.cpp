
#include "connectionmanager.h"
#include "wificonnection.h"
#include "bluetoothconnection.h"

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent)
{
    connectToWifi();
}

ConnectionManager::~ConnectionManager()
{
    emit stopConnection();
}

void ConnectionManager::runApp()
{
    emit runAppSignal();
}

void ConnectionManager::killApp()
{
    emit killAppSignal();
}

void ConnectionManager::sendFile(QString file)
{
    emit sendFileSignal(file);
}

void ConnectionManager::connectToWifi()
{
    if (m_connection != 0) {
        m_connection->deleteLater();
    }
    setConnection(new WiFiConnection);
}

void ConnectionManager::connectToBluetooth()
{
    if (m_connection != 0) {
        m_connection->deleteLater();
    }
    setConnection(new BluetoothConnection);
}

void ConnectionManager::setConnection(AbstractConnection *connection)
{

    m_connection = connection;
    QThread *thread = new QThread;

    m_connection->moveToThread(thread);
    connect(thread, SIGNAL(started()), m_connection, SLOT(init()));
    connect(this, SIGNAL(stopConnection()), m_connection, SLOT(deleteLater()));
    connect(m_connection, SIGNAL(destroyed()), thread, SLOT(quit()));
    connect(m_connection, SIGNAL(destroyed()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), m_connection, SLOT(deleteLater()));
    thread->start();

    connectSignals();
}

void ConnectionManager::connectSignals()
{
    connect(this, SIGNAL(runAppSignal()), m_connection, SLOT(runApp()));
    connect(this, SIGNAL(killAppSignal()), m_connection, SLOT(killApp()));
    connect(this, SIGNAL(sendFileSignal(QString)), m_connection, SLOT(sendFile(QString)));

    connect(m_connection, SIGNAL(appSent(bool)), this, SIGNAL(appSent(bool)));
    connect(m_connection, SIGNAL(appStarted(bool)), this, SIGNAL(appStarted(bool)));
    connect(m_connection, SIGNAL(appKilled(bool)), this, SIGNAL(appKilled(bool)));
    connect(m_connection, SIGNAL(statusUpdated(StatusInfo)), this, SIGNAL(statusUpdated(StatusInfo)));
    connect(m_connection, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}
