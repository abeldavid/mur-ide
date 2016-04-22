#ifndef BLUETOOTHCONNECTION_H
#define BLUETOOTHCONNECTION_H

#include <QObject>
#include "abstractconnection.h"

class BluetoothConnection : public AbstractConnection
{
    Q_OBJECT
public:
    explicit BluetoothConnection(QObject *parent = 0);
    ~BluetoothConnection();
//signals:
//    void bluetoothDevicesFailed();
public slots:
    void init();
    void runApp();
    void killApp();
    void sendFile(QString file);
    void bluetoothDevicesRequested();

private slots:
    void updateRobotInfo();
    void onDisconected();

private:
    const int m_reqTimeout = 2000;
    const int m_reqOption = 0;
    QString m_reqAddr;
    void* m_zmqContext;
    void* m_zmqReqSoc;
    bool sendCommand(uint8_t command);
    void initReqSocket();
    void recreateReqSocket();
};

#endif // BLUETOOTHCONNECTION_H
