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
    qDebug() << "not implemented on Bluetooth";
//    if (sendCommand(m_commandSend)) {
//        emit appSent(true);
//    }
//    else {
//        emit appSent(false);
//    }

}

void BluetoothConnection::bluetoothDevicesRequested()
{
    uint8_t retryNumber = 5;
    if (!m_isConnected) {
        recreateReqSocket();
    }
    uint8_t cmd = 200; // get device list command
    zmq_msg_t ideCmdData;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    while (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0) && retryNumber > 0) {
        recreateReqSocket();
        QThread::msleep(500);
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
        QThread::msleep(500);
        --retryNumber;
    }
    if (retryNumber > 0) {
        QByteArray byteArray(static_cast<char*>(zmq_msg_data(&devicesMsg)), zmq_msg_size(&devicesMsg));
        emit bluetoothDevicesReceived(byteArray);
    }
    zmq_msg_close(&devicesMsg);
}

void BluetoothConnection::connectToDevice(QString deviceMAC)
{
//    qDebug() << deviceMAC;
    uint8_t retryNumber = 5;
    if (!m_isConnected) {
        recreateReqSocket();
    }
    // send command
    uint8_t cmd = 210; // get device list command
    zmq_msg_t ideCmdData;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    while (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0) && retryNumber > 0) {
        recreateReqSocket();
        QThread::msleep(500);
        --retryNumber;
    }
    zmq_msg_close(&ideCmdData);
    if (retryNumber == 0){
        return;
    }

    // send MAC
    zmq_msg_t macDataMsg;
    zmq_msg_init_size(&macDataMsg, deviceMAC.size());
    memcpy(zmq_msg_data(&macDataMsg), deviceMAC.toStdString().c_str(), deviceMAC.size());

    while (-1 == zmq_msg_send(&macDataMsg, m_zmqReqSoc, 0) && retryNumber > 0) {
        recreateReqSocket();
        QThread::msleep(500);
        --retryNumber;
    }
    zmq_msg_close(&macDataMsg);
    if (retryNumber == 0){
        return;
    }

    // receive result
    zmq_msg_t resultMsg;

    zmq_msg_init(&resultMsg);
    retryNumber = 5;
    while (-1 == zmq_msg_recv(&resultMsg, m_zmqReqSoc, 0) && retryNumber > 0) {
        QThread::msleep(500);
        --retryNumber;
    }
    if (retryNumber > 0) {
        QByteArray byteArray(static_cast<char*>(zmq_msg_data(&resultMsg)), zmq_msg_size(&resultMsg));
        if (QString::fromStdString(byteArray.toStdString()) == "Success") {
            emit bluetoothDeviceConnected();
        }
    }
    zmq_msg_close(&resultMsg);
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
        recreateReqSocket();
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

