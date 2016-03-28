#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include "filetransfer/abstractconnection.h"
#include "logic/processrunner.h"

#include <QThread>
#include <QTimer>

static const std::size_t DEVICES_NUM = 6;

struct StatusInfo
{
    uint8_t devicesTypes[DEVICES_NUM];
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float depth = 0.0f;
    uint8_t leak = 0;
    uint8_t version = 0;
    uint8_t cameras = 0;
};

Q_DECLARE_METATYPE(StatusInfo)


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
    bool m_isConnected;
    void* m_zmqContext;
    void* m_zmqReqSoc;
    void* m_zmqInfoSub;
    QTimer *m_connectionTimeout;
    QTimer *m_updateDeviceListTimer;
    QThread *m_connectionThread;
};

#endif // WIFICONNECTION_H
