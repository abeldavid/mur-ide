#include <QDebug>
#include <QThread>
//#include <QIODevice>

#include "connectionmanager.h"
#include "wificonnection.h"
#include "bluetoothconnection.h"

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent),
    m_connectionExists(false),
    m_bluetoothBinaryName("bluetooth\\bluetooth_connector.exe")
{
    createBluetoothProcess();
//    connect(m_bluetoothProcess, SIGNAL(finished(int , QProcess::ExitStatus)),
//            this, SLOT(onBluetoothProcessExit(int, QProcess::ExitStatus)));
    connectToWifi();
}

ConnectionManager::~ConnectionManager()
{
    emit stopConnection();
    m_bluetoothProcess->kill();
    m_bluetoothProcess->waitForFinished(1000);
    m_bluetoothProcess->deleteLater();
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
    setConnection(new WiFiConnection);
}

void ConnectionManager::connectToBluetooth()
{
    m_bluetoothProcess->waitForStarted();
    setConnection(new BluetoothConnection);
    connect(this, SIGNAL(requestBluetoothDevices()), m_connection, SLOT(bluetoothDevicesRequested()));
    connect(m_connection, SIGNAL(bluetoothDevicesReceived(QByteArray)), this, SLOT(bluetoothDevicesReceived(QByteArray)));
    connect(this, SIGNAL(connectToBluetoothDevice(QString)), m_connection, SLOT(connectToDevice(QString)));
    emit requestBluetoothDevices();
}

void ConnectionManager::bluetoothDevicesReceived(QByteArray devices)
{
    emit receivedBluetoothDevices(devices);
}

void ConnectionManager::bluetoothDeviceSelected(QString deviceMAC)
{
    emit connectToBluetoothDevice(deviceMAC);
}

void ConnectionManager::setConnection(AbstractConnection *connection)
{
    if (m_connectionExists) {
        m_connection->deleteLater();
    }
    m_connectionExists = true;
    m_connection = connection;
    QThread *thread = new QThread;

    m_connection->moveToThread(thread);
    connect(thread, SIGNAL(started()), m_connection, SLOT(init()));
    connect(m_connection, SIGNAL(destroyed()), thread, SLOT(quit()));
    connect(this, SIGNAL(stopConnection()), m_connection, SLOT(deleteLater()));
//    connect(m_connection, SIGNAL(destroyed()), thread, SLOT(deleteLater()));
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

void ConnectionManager::createBluetoothProcess()
{
    m_bluetoothProcess = new QProcess;
    m_bluetoothProcess->start(m_bluetoothBinaryName);
}

void ConnectionManager::onBluetoothProcessExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    if (exitStatus == QProcess::CrashExit) {
        qDebug() << "crash, restarting";
        createBluetoothProcess();
    }
}

