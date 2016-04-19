#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include "connections/abstractconnection.h"

class WiFiConnection : public AbstractConnection
{
    Q_OBJECT
public:
    explicit WiFiConnection(QObject *parent = 0);
    ~WiFiConnection();
public slots:
    void init();
//    void stop();
    void runApp();
    void killApp();
    void sendFile(QString file);
private slots:
    void updateRobotInfo();
    void onDisconected();
private:
    const int m_reqTimeout = 2000;
    const int m_reqOption = 0;
    const int m_subHWM = 5;
    QString m_subAddr;
    QString m_reqAddr;
    void* m_zmqContext;
    void* m_zmqReqSoc;
    void* m_zmqInfoSub;
    bool sendCommand(uint8_t command);
    void initSubSocket();
    void initReqSocket();
    void recreateReqSocket();
};

#endif // WIFICONNECTION_H
