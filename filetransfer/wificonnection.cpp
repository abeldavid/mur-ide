#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include <thread>
#include <zmq.h>

#include "wificonnection.h"
#include "settingsmanager.h"

WiFiConnection::WiFiConnection(QObject *parent) :
    QObject(parent),
    m_zmqContext(zmq_ctx_new()),
    m_zmqReqSoc(zmq_socket(m_zmqContext, ZMQ_REQ)),
    m_zmqInfoSub(zmq_socket(m_zmqContext, ZMQ_SUB)),
    m_connectionTimeout(new QTimer(this)),
    m_updateDeviceListTimer(new QTimer(this))
   // m_connectionThread(new QThread(parent))
{
    int timeout = 2000;
    int option = 0;

    zmq_connect(m_zmqInfoSub, "tcp://192.168.42.1:2052");
    zmq_connect(m_zmqReqSoc,  "tcp://192.168.42.1:7350");

    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqInfoSub, ZMQ_SUBSCRIBE, 0, 0);


    m_connectionTimeout->setInterval(7000);
    m_updateDeviceListTimer->setInterval(1000);

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
    /*
    if (!m_isConnected) {
        emit appSend(false);
        return;
    }
    */
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
    /*
    if (!m_isConnected) {
        emit appSend(false);
        return;
    }
    */
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

void WiFiConnection::send(QString file)
{
    /*
    if (!m_isConnected) {
        emit appSend(false);
        return;
    }
    */
    zmq_msg_t ideCmdData;
    uint8_t cmd = 10;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSend(false);
        return;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        recreateSockets();
        emit appSend(false);
        return;
    }
    zmq_msg_close(&serverCmdData);
    QFileInfo fInfo(file);
    zmq_msg_t pathToBinary;
    QString path = "/home/root/apps/" + fInfo.fileName();

    zmq_msg_init_size(&pathToBinary, path.size());
    memcpy(zmq_msg_data(&pathToBinary), path.toStdString().c_str(), path.size());

    if (-1 == zmq_msg_send(&pathToBinary, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSend(false);
        return;
    }
    zmq_msg_close(&pathToBinary);

    zmq_msg_t serverPathData;
    zmq_msg_init(&serverPathData);
    if (-1 == zmq_msg_recv(&serverPathData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSend(false);
        return;
    }
    zmq_msg_close(&serverPathData);

    QFile bin(file);
    if (!bin.open(QIODevice::ReadOnly)) {
        emit appSend(false);
        return;
    }
    QByteArray blob = bin.readAll();

    zmq_msg_t binaryData;
    zmq_msg_init_size(&binaryData, blob.size());
    memcpy(zmq_msg_data(&binaryData), (void*)blob.data(), blob.size());
    if (-1 == zmq_msg_send(&binaryData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSend(false);
        return;
    }

    zmq_msg_t serverBinData;
    zmq_msg_init(&serverBinData);
    if (-1 == zmq_msg_recv(&serverBinData, m_zmqReqSoc, 0)) {
        recreateSockets();
        emit appSend(false);
        return;
    }
    zmq_msg_close(&serverBinData);
    emit appSend(true);
}

void WiFiConnection::updateRobotInfo()
{
    zmq_msg_t robotInfo;
    zmq_msg_init(&robotInfo);
    if (-1 != zmq_msg_recv(&robotInfo, m_zmqInfoSub, ZMQ_DONTWAIT)) {
        memcpy(&m_robotInfo, zmq_msg_data(&robotInfo), sizeof(StatusInfo));
        //Restarting connection timeout;
        qDebug() << "CC";
        m_connectionTimeout->start();
        m_isConnected = true;
        emit statusUpdated(m_robotInfo);
    }
}

void WiFiConnection::onDisconected()
{
    m_isConnected = false;
    qDebug() << "DC";
    recreateSockets();
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
    zmq_connect(m_zmqReqSoc, "tcp://192.168.42.1:7350");
}

