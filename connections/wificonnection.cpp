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
    m_zmqContext(zmq_ctx_new())
{
    m_subAddr = "tcp://" + SettingsManager::instance().ipAddress() + ":2052";
    m_reqAddr = "tcp://" + SettingsManager::instance().ipAddress() + ":7350";

    m_zmqReqSoc = zmq_socket(m_zmqContext, ZMQ_REQ);
    initReqSocket();

    m_zmqInfoSub = zmq_socket(m_zmqContext, ZMQ_SUB);
    initSubSocket();

    initTimers();
}

WiFiConnection::~WiFiConnection()
{

}

void WiFiConnection::runApp()
{
    if (sendCommand(m_commandRun)) {
        emit appStarted(true);
    }
    else {
        emit appStarted(false);
    }
}

void WiFiConnection::killApp()
{
    if (sendCommand(m_commandKill)) {
        emit appKilled(true);
    }
    else {
        emit appKilled(false);
    }
}

void WiFiConnection::sendFile(QString file)
{
    if (!sendCommand(m_commandSend)) {
        emit appSent(false);
        return;
    }
    QFileInfo fInfo(file);
    zmq_msg_t pathToBinary;
    QString path = m_binaryPath + fInfo.fileName();

    zmq_msg_init_size(&pathToBinary, path.size());
    memcpy(zmq_msg_data(&pathToBinary), path.toStdString().c_str(), path.size());

    if (-1 == zmq_msg_send(&pathToBinary, m_zmqReqSoc, 0)) {
        recreateReqSocket();
        emit appSent(false);
        return;
    }
    zmq_msg_close(&pathToBinary);

    zmq_msg_t serverPathData;
    zmq_msg_init(&serverPathData);
    if (-1 == zmq_msg_recv(&serverPathData, m_zmqReqSoc, 0)) {
        recreateReqSocket();
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
        recreateReqSocket();
        emit appSent(false);
        return;
    }

    zmq_msg_t serverBinData;
    zmq_msg_init(&serverBinData);
    if (-1 == zmq_msg_recv(&serverBinData, m_zmqReqSoc, 0)) {
        recreateReqSocket();
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
    recreateReqSocket();
    emit disconnected();
}

bool WiFiConnection::sendCommand(uint8_t cmd)
{
    if (!m_isConnected) {
        recreateReqSocket();
        return false;
    }
    zmq_msg_t ideCmdData;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        recreateReqSocket(); // TODO: check if correct (was not in run and kill, but was in send file)
        return false;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        recreateReqSocket(); // TODO: -||-
        return false;
    }
    zmq_msg_close(&serverCmdData);
    return true;
}

void WiFiConnection::initSubSocket()
{
    zmq_setsockopt(m_zmqInfoSub, ZMQ_SUBSCRIBE, 0, 0);
    zmq_setsockopt(m_zmqInfoSub, ZMQ_RCVHWM, &m_subHWM, sizeof(int));
    zmq_connect(m_zmqInfoSub, m_subAddr.toStdString().c_str());
}

void WiFiConnection::initReqSocket()
{
    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &m_reqTimeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &m_reqOption, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &m_reqOption, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &m_reqTimeout, sizeof(int));
    zmq_connect(m_zmqReqSoc,  m_reqAddr.toStdString().c_str());
}

void WiFiConnection::recreateReqSocket()
{
    zmq_close(m_zmqReqSoc);
    zmq_socket(m_zmqContext, ZMQ_REQ);
    initReqSocket();
}


