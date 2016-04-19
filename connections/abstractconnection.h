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
    virtual void init() = 0;
    virtual void runApp() = 0;
    virtual void killApp() = 0;
    virtual void sendFile(QString file) = 0;
signals:
    void appKilled(bool);
    void appStarted(bool);
    void appSent(bool);
    void statusUpdated(StatusInfo);
    void disconnected();

protected slots:
    virtual void updateRobotInfo() = 0;
    virtual void onDisconected() = 0;
protected:
    const uint8_t m_commandSend = 10;
    const uint8_t m_commandRun = 30;
    const uint8_t m_commandKill = 40;
    const int m_updateInterval = 500;
    const int m_disconnectInterval = 2000;
    StatusInfo m_robotInfo;
    const QString m_binaryPath;
    bool m_isConnected = false;
    QTimer *m_connectionTimeout;
    QTimer *m_updateDevicesTimer;
    void initTimers();
};

#endif // ABSTRACTCONNECTION_H
