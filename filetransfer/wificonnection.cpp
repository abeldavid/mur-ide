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
    m_zmqReqSoc(zmq_socket(m_zmqContext, ZMQ_REQ))
{
    int timeout = 7000;
    int option = 0;

    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &timeout, sizeof(int));
    zmq_connect(m_zmqReqSoc, "tcp://192.168.42.1:7350");
}

WiFiConnection::~WiFiConnection()
{
}

void WiFiConnection::prepare()
{
    /*
    if (!m_isReady) {
        return;
    }
    m_isReady = false;
    m_currentExec = CURRENT_EXEC::PREPARE;
    m_processRunner->setPath(SettingsManager::instance().plinkPath());
    QStringList arg;
    arg << "-ssh" << SettingsManager::instance().userLogin() + "@" + SettingsManager::instance().ipAddress();
    arg << "-l" << SettingsManager::instance().userLogin();
    arg << "-pw" << SettingsManager::instance().userPassword();
    arg << "echo";
    m_processRunner->setArguments(arg);
    emit run();
    */
}

QStringList WiFiConnection::appList()
{
    return QStringList();
}

bool WiFiConnection::runApp()
{
    zmq_msg_t ideCmdData;
    uint8_t cmd = 30;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        qDebug() << "Timeout send CMD";
        return false;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        qDebug() << "Timeout RECV";
        return false;
    }
    zmq_msg_close(&serverCmdData);
    return true;
}

bool WiFiConnection::killApp()
{
    zmq_msg_t ideCmdData;
    uint8_t cmd = 40;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        qDebug() << "Timeout send CMD";
        return false;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        qDebug() << "Timeout RECV";
        return false;
    }
    zmq_msg_close(&serverCmdData);
    return true;
}

void WiFiConnection::stopAppByName(QString bin)
{
    Q_UNUSED(bin)
}

bool WiFiConnection::send(QString file)
{
    zmq_msg_t ideCmdData;
    uint8_t cmd = 10;
    zmq_msg_init_size(&ideCmdData, sizeof(uint8_t));
    memcpy(zmq_msg_data(&ideCmdData), &cmd, sizeof(uint8_t));

    if (-1 == zmq_msg_send(&ideCmdData, m_zmqReqSoc, 0)) {
        recreateSockets();
        qDebug() << "Timeout send CMD";
        return false;
    }
    zmq_msg_close(&ideCmdData);

    zmq_msg_t serverCmdData;
    zmq_msg_init(&serverCmdData);
    if (-1 == zmq_msg_recv(&serverCmdData, m_zmqReqSoc, 0))
    {
        recreateSockets();
        qDebug() << "Timeout RECV";
        return false;
    }
    zmq_msg_close(&serverCmdData);
    QFileInfo fInfo(file);
    zmq_msg_t pathToBinary;
    QString path = "/home/root/apps/" + fInfo.fileName();

    zmq_msg_init_size(&pathToBinary, path.size());
    memcpy(zmq_msg_data(&pathToBinary), path.toStdString().c_str(), path.size());

    if (-1 == zmq_msg_send(&pathToBinary, m_zmqReqSoc, 0)) {
        recreateSockets();
        qDebug() << "Timeout";
        return false;
    }
    zmq_msg_close(&pathToBinary);

    zmq_msg_t serverPathData;
    zmq_msg_init(&serverPathData);
    if (-1 == zmq_msg_recv(&serverPathData, m_zmqReqSoc, 0)) {
        recreateSockets();
        qDebug() << "Timeout";
        return false;
    }
    zmq_msg_close(&serverPathData);

    QFile bin(file);
    if (!bin.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray blob = bin.readAll();

    zmq_msg_t binaryData;
    zmq_msg_init_size(&binaryData, blob.size());
    memcpy(zmq_msg_data(&binaryData), (void*)blob.data(), blob.size());
    if (-1 == zmq_msg_send(&binaryData, m_zmqReqSoc, 0)) {
        recreateSockets();
        qDebug() << "Timeout";
        return false;
    }

    zmq_msg_t serverBinData;
    zmq_msg_init(&serverBinData);
    if (-1 == zmq_msg_recv(&serverBinData, m_zmqReqSoc, 0)) {
        recreateSockets();
        qDebug() << "Timeout";
        return false;
    }
    zmq_msg_close(&serverBinData);
    return true;
}

void WiFiConnection::onExecFinished(int retCode)
{
    Q_UNUSED(retCode)
}

void WiFiConnection::recreateSockets()
{
    int timeout = 3000;
    int option = 0;
    qDebug() << "SOCK";
    zmq_close(m_zmqReqSoc);

    zmq_socket(m_zmqContext, ZMQ_REQ);
    zmq_setsockopt(m_zmqReqSoc, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_RELAXED, &option, sizeof(int));
    zmq_setsockopt(m_zmqReqSoc, ZMQ_REQ_CORRELATE, &option, sizeof(int));

    zmq_setsockopt(m_zmqReqSoc, ZMQ_RCVTIMEO, &timeout, sizeof(int));
    zmq_connect(m_zmqReqSoc, "tcp://192.168.42.1:7350");
}

