#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include "connections/abstractconnection.h"
#include "logic/processrunner.h"
#include "statusinfo.h"

#include <QThread>
#include <QTimer>

class WiFiConnection : public QObject
{
    Q_OBJECT
public:
    explicit WiFiConnection(QObject *parent = 0);
    ~WiFiConnection();

signals:
    void run();
    void onExecOutput(QString);
    void appKilled(bool);
    void appStarted(bool);
    void appSend(bool);
    void statusUpdated(StatusInfo);
    void disconnected();
public slots:
    void runApp();
    void killApp();
    void send(QString file);
private slots:
    void updateRobotInfo();
    void onDisconected();
private:
    StatusInfo m_robotInfo;
    void recreateSockets();
    QString m_binaryPath;
    bool m_isConnected = false;
    void* m_zmqContext;
    void* m_zmqReqSoc;
    void* m_zmqInfoSub;
    QTimer *m_connectionTimeout;
    QTimer *m_updateDeviceListTimer;
    QThread *m_connectionThread;
};

#endif // WIFICONNECTION_H
