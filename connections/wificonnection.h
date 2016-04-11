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
    void runApp();
    void killApp();
    void send(QString file);
private slots:
    void updateRobotInfo();
    void onDisconected();
private:
    void recreateSockets();
};

#endif // WIFICONNECTION_H
