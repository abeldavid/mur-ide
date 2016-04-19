#include <QDebug>
#include <zmq.h>

#include "bluetoothconnection.h"
#include "settingsmanager.h"

BluetoothConnection::BluetoothConnection(QObject *parent) :
    AbstractConnection(parent),
    m_zmqContext(zmq_ctx_new())
{
    m_reqAddr = "tcp://localhost:5556";
}

void BluetoothConnection::init()
{
    m_zmqReqSoc = zmq_socket(m_zmqContext, ZMQ_REQ);
    initReqSocket();
}

BluetoothConnection::~BluetoothConnection()
{

}

void BluetoothConnection::runApp()
{
    if (sendCommand(m_commandRun)) {
        emit appStarted(true);
    }
    else {
        emit appStarted(false);
    }
}

void BluetoothConnection::killApp()
{
    qDebug() << "sending kill";
//    if (sendCommand(m_commandKill)) {
    if (sendCommand(100)) {
        qDebug() << "sendCommand Ok";
        emit appKilled(true);
    }
    else {
        emit appKilled(false);
    }
}

void BluetoothConnection::sendFile(QString file) {

}

void BluetoothConnection::updateRobotInfo() {

}

void BluetoothConnection::onDisconected()
{
    qDebug() << "connection timeout";
    m_isConnected = false;
    recreateReqSocket();
    emit disconnected();
}

bool BluetoothConnection::sendCommand(uint8_t cmd)
{
    zmq_msg_t ideCmdData;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        qDebug() << "return 2";
        recreateReqSocket(); // TODO: check if correct (was not in run and kill, but was in send file)
        return false;
    }
    zmq_msg_close(&ideCmdData);

//    zmq_msg_t serverCmdData;
//    zmq_msg_init(&serverCmdData);
//    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
//    {
//        qDebug() << "return 3";
//        recreateReqSocket(); // TODO: -||-
//        return false;
//    }
//    zmq_msg_close(&serverCmdData);

    zmq_msg_t robotInfo;
    zmq_msg_init(&robotInfo);
    if (-1 != zmq_msg_recv(&robotInfo, m_zmqReqSoc, 0)) {
        qDebug() << "reply Ok";
        memcpy(&m_robotInfo, zmq_msg_data(&robotInfo), sizeof(StatusInfo));
        //Restarting connection timeout;
//        m_connectionTimeout->start();
//        m_isConnected = true;
        qDebug() << "emitting info";
        emit statusUpdated(m_robotInfo);
    }
    qDebug() << "all OK";
    zmq_msg_close(&robotInfo);
    return true;
}

void BluetoothConnection::initReqSocket()
{
    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &m_reqTimeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &m_reqOption, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &m_reqOption, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &m_reqTimeout, sizeof(int));
    zmq_connect(m_zmqReqSoc,  m_reqAddr.toStdString().c_str());
}

void BluetoothConnection::recreateReqSocket()
{
    zmq_close(m_zmqReqSoc);
    zmq_socket(m_zmqContext, ZMQ_REQ);
    initReqSocket();
}

//void WiFiConnection::updateRobotInfo()
//{
//    qDebug() << "update info";
//    zmq_msg_t robotInfo;
//    zmq_msg_init(&robotInfo);
//    if (-1 != zmq_msg_recv(&robotInfo, m_zmqInfoSub, ZMQ_DONTWAIT)) {
//        memcpy(&m_robotInfo, zmq_msg_data(&robotInfo), sizeof(StatusInfo));
//        //Restarting connection timeout;
//        m_connectionTimeout->start();
//        m_isConnected = true;
//        emit statusUpdated(m_robotInfo);
//    }
//}

