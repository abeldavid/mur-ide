#ifndef ABSTRACTCONNECTION_H
#define ABSTRACTCONNECTION_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QThread>
#include "statusinfo.h"

class AbstractConnection : public QObject
{
    Q_OBJECT
public:
    explicit AbstractConnection(QObject *parent = 0);
    virtual ~AbstractConnection();
public slots:
    virtual void runApp() = 0;
    virtual void killApp() = 0;
    virtual void send(QString file) = 0;
signals:
    void run();
    void onExecOutput(QString);
    void appKilled(bool);
    void appStarted(bool);
    void appSend(bool);
    void statusUpdated(StatusInfo);
    void disconnected();
protected slots:
    virtual void updateRobotInfo() = 0;
    virtual void onDisconected() = 0;
    virtual void recreateSockets() = 0;
protected:
    StatusInfo m_robotInfo;
    QString m_binaryPath;
    bool m_isConnected = false;
    void* m_zmqContext;
    void* m_zmqReqSoc;
    void* m_zmqInfoSub;
    QTimer *m_connectionTimeout;
    QTimer *m_updateDeviceListTimer;
    QThread *m_connectionThread;
};

#endif // ABSTRACTCONNECTION_H
