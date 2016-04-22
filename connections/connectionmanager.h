#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QProcess>
#include  <QString>

#include "abstractconnection.h"

// more like ConnectionProxy
class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionManager(QObject *parent = 0);
    ~ConnectionManager();

public slots:
    void runApp();
    void killApp();
    void sendFile(QString file);
    void connectToWifi();
    void connectToBluetooth();
    void bluetoothDevicesReceived(QByteArray devices);

signals:
    void runAppSignal();
    void killAppSignal();
    void sendFileSignal(QString file);
    void appKilled(bool);
    void appStarted(bool);
    void appSent(bool);
    void statusUpdated(StatusInfo);
    void disconnected();
    void stopConnection();
    void requestBluetoothDevices();
    void receivedBluetoothDevices(QByteArray devices);

private:
    AbstractConnection* m_connection;
    bool m_connectionExists;
    QProcess* m_bluetoothProcess;
    const QString m_bluetoothBinaryName;

private slots:
    void setConnection(AbstractConnection *connection);
    void connectSignals();
    void createBluetoothProcess();
    void onBluetoothProcessExit(int exitCode, QProcess::ExitStatus exitStatus);
//    void disconnectSignals();

};

#endif // CONNECTIONMANAGER_H
