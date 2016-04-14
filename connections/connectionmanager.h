#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include "abstractconnection.h"

// more like ConnectionProxy
class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionManager(QObject *parent = 0);
//    ~ConnectionManager();

public slots:
    void runApp();
    void killApp();
    void sendFile(QString file);
    void connectToWifi();
    void connectToBluetooth();

signals:
    void appKilled(bool);
    void appStarted(bool);
    void appSent(bool);
    void statusUpdated(StatusInfo);
    void disconnected();

private:
    AbstractConnection *m_connection;

private slots:
    void connectSignals();
//    void disconnectSignals();

};

#endif // CONNECTIONMANAGER_H
