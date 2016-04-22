#include <QDebug>
#include <zmq.h>
#include <QThread>

#include "bluetoothconnection.h"
#include "settingsmanager.h"

BluetoothConnection::BluetoothConnection(QObject *parent) :
    AbstractConnection(parent)
{

}

void BluetoothConnection::init()
{
    m_zmqContext = zmq_ctx_new();
    m_reqAddr = "tcp://localhost:5556";
    m_zmqReqSoc = zmq_socket(m_zmqContext, ZMQ_REQ);
    initReqSocket();
    initTimers();
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
    if (sendCommand(m_commandKill)) {
        emit appKilled(true);
    }
    else {
        emit appKilled(false);
    }
}

void BluetoothConnection::sendFile(QString file) {
    qDebug() << "not implemented";
    if (sendCommand(m_commandSend)) {
        emit appSent(true);
    }
    else {
        emit appSent(false);
    }

}

void BluetoothConnection::bluetoothDevicesRequested()
{
    uint8_t retryNumber = 5;
    if (!m_isConnected) {
        recreateReqSocket();
    }
    uint8_t cmd = 200;
    zmq_msg_t ideCmdData;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    while (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0) && retryNumber > 0) {
        recreateReqSocket();
        QThread::msleep(1000);
//        qDebug() << retryNumber;
        --retryNumber;
    }
    zmq_msg_close(&ideCmdData);
    if (retryNumber == 0){
        return;
    }

    zmq_msg_t devicesMsg;

    zmq_msg_init(&devicesMsg);
    retryNumber = 5;
    while (-1 == zmq_msg_recv(&devicesMsg, m_zmqReqSoc, 0) && retryNumber > 0) {
        QThread::msleep(100);
//        qDebug() << "retry receive";
        --retryNumber;
    }
    if (retryNumber > 0) {
//    if (-1 != zmq_msg_recv(&devicesMsg, m_zmqReqSoc, 0)) {
//        std::string rpl = std::string(static_cast<char*>(zmq_msg_data(&devicesMsg)), zmq_msg_size(&devicesMsg));
        QByteArray byteArray(static_cast<char*>(zmq_msg_data(&devicesMsg)), zmq_msg_size(&devicesMsg));
        emit bluetoothDevicesReceived(byteArray);
//        emit bluetoothDevicesReceived(QByteArray(rpl.c_str(), rpl.length()));
    }
//    else {
//        emit bluetoothDevicesFailed();
//    }
    zmq_msg_close(&devicesMsg);
}

void BluetoothConnection::onDisconected()
{
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
    m_isConnected = true;
}

void BluetoothConnection::updateRobotInfo()
{
    if (!m_isConnected) {
        recreateReqSocket();
        return;
    }
    uint8_t cmd = 100;
    zmq_msg_t ideCmdData;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        recreateReqSocket();
        return;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t robotInfo;
    zmq_msg_init(&robotInfo);

    if (-1 != zmq_msg_recv(&robotInfo, m_zmqReqSoc, 0)) {
        memcpy(&m_robotInfo, zmq_msg_data(&robotInfo), sizeof(StatusInfo));
        //Restarting connection timeout;
        m_connectionTimeout->start();
        m_isConnected = true;
        emit statusUpdated(m_robotInfo);
    }
}

