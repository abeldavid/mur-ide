#include "abstractconnection.h"

AbstractConnection::AbstractConnection(QObject *parent) :
    QObject(parent),
    m_binaryPath("/home/root/apps/"),
    m_connectionTimeout(new QTimer(this)),
    m_updateDevicesTimer(new QTimer(this))
{

}

AbstractConnection::~AbstractConnection()
{

}
