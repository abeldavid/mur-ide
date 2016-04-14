#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include <thread>
#include <zmq.h>
#include <numeric>

#include "wificonnection.h"
#include "settingsmanager.h"

WiFiConnection::WiFiConnection(QObject *parent) :
    AbstractConnection(parent),
    m_zmqContext(zmq_ctx_new()),
    m_zmqReqSoc(zmq_socket(m_zmqContext, ZMQ_REQ)),
    m_zmqInfoSub(zmq_socket(m_zmqContext, ZMQ_SUB))
{
    int timeout = 2000;
    int option = 0;
    int hwm = 5;

    QString ipSub = "tcp://" + SettingsManager::instance().ipAddress() + ":2052";
    QString ipReq = "tcp://" + SettingsManager::instance().ipAddress() + ":7350";

    zmq_connect(m_zmqInfoSub, ipSub.toStdString().c_str());
    zmq_connect(m_zmqReqSoc,  ipReq.toStdString().c_str());

    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqInfoSub, ZMQ_SUBSCRIBE, 0, 0);
    zmq_setsockopt(m_zmqInfoSub, ZMQ_RCVHWM, &hwm, sizeof(int));

    m_connectionTimeout->setInterval(2000);
    m_updateDeviceListTimer->setInterval(500);

    QObject::connect(m_connectionTimeout, SIGNAL(timeout()), this, SLOT(onDisconected()));
    QObject::connect(m_updateDeviceListTimer, SIGNAL(timeout()), this, SLOT(updateRobotInfo()));

    m_updateDeviceListTimer->start();
    m_connectionTimeout->start();
}

WiFiConnection::~WiFiConnection()
{

}

void WiFiConnection::runApp()
{

    if (!m_isConnected) {
        recreateSockets();
        emit appStarted(false);
        return;
    }

    zmq_msg_t ideCmdData;
    uint8_t cmd = 30;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        emit appStarted(false);
        return;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        emit appStarted(false);
        return;
    }
    zmq_msg_close(&serverCmdData);
    emit appStarted(true);
}

void WiFiConnection::killApp()
{

    if (!m_isConnected) {
        recreateSockets();
        emit appKilled(false);
        return;
    }

    qDebug() << "About to kill";
    zmq_msg_t ideCmdData;
    uint8_t cmd = 40;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        emit appKilled(false);
        return;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        emit appKilled(false);
        return;

    }
    zmq_msg_close(&serverCmdData);
    emit appKilled(true);
}

void WiFiConnection::sendFile(QString file)
{

    if (!m_isConnected) {
        recreateSockets();
        emit appSent(false);
        return;
    }

    zmq_msg_t ideCmdData;
    uint8_t cmd = 10;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSent(false);
        return;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        recreateSockets();
        emit appSent(false);
        return;
    }
    zmq_msg_close(&serverCmdData);
    QFileInfo fInfo(file);
    zmq_msg_t pathToBinary;
    QString path = m_binaryPath + fInfo.fileName();

    zmq_msg_init_size(&pathToBinary, path.size());
    memcpy(zmq_msg_data(&pathToBinary), path.toStdString().c_str(), path.size());

    if (-1 == zmq_msg_send(&pathToBinary, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSent(false);
        return;
    }
    zmq_msg_close(&pathToBinary);

    zmq_msg_t serverPathData;
    zmq_msg_init(&serverPathData);
    if (-1 == zmq_msg_recv(&serverPathData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSent(false);
        return;
    }
    zmq_msg_close(&serverPathData);

    QFile bin(file);
    if (!bin.open(QIODevice::ReadOnly)) {
        emit appSent(false);
        return;
    }
    QByteArray blob = bin.readAll();

    zmq_msg_t binaryData;
    zmq_msg_init_size(&binaryData, blob.size());
    memcpy(zmq_msg_data(&binaryData), (void*)blob.data(), blob.size());
    if (-1 == zmq_msg_send(&binaryData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSent(false);
        return;
    }

    zmq_msg_t serverBinData;
    zmq_msg_init(&serverBinData);
    if (-1 == zmq_msg_recv(&serverBinData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSent(false);
        return;
    }
    zmq_msg_close(&serverBinData);
    emit appSent(true);
}

void WiFiConnection::updateRobotInfo()
{
    qDebug() << "update info";
    zmq_msg_t robotInfo;
    zmq_msg_init(&robotInfo);
    if (-1 != zmq_msg_recv(&robotInfo, m_zmqInfoSub, ZMQ_DONTWAIT)) {
        memcpy(&m_robotInfo, zmq_msg_data(&robotInfo), sizeof(StatusInfo));
        //Restarting connection timeout;
        m_connectionTimeout->start();
        m_isConnected = true;
        emit statusUpdated(m_robotInfo);
    }
}

void WiFiConnection::onDisconected()
{
    qDebug() << "connection timeout";
    m_isConnected = false;
    recreateSockets();
    emit disconnected();
}

void WiFiConnection::recreateSockets()
{
    int timeout = 2000;
    int option = 0;

    zmq_close(m_zmqReqSoc);

    zmq_socket(m_zmqContext, ZMQ_REQ);
    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &timeout, sizeof(int));

    QString ipReq = "tcp://" + SettingsManager::instance().ipAddress() + ":7350";
    zmq_connect(m_zmqReqSoc,  ipReq.toStdString().c_str());
}

