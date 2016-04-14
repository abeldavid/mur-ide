#include "abstractconnection.h"
#include <zmq.h>

AbstractConnection::AbstractConnection(QObject *parent) :
    QObject(parent),
    m_binaryPath("/home/root/apps/"),
    m_zmqContext(zmq_ctx_new()),
    m_zmqReqSoc(zmq_socket(m_zmqContext, ZMQ_REQ)),
    m_zmqInfoSub(zmq_socket(m_zmqContext, ZMQ_SUB)),
    m_connectionTimeout(new QTimer(this)),
    m_updateDeviceListTimer(new QTimer(this))
{

}

AbstractConnection::~AbstractConnection()
{

}

