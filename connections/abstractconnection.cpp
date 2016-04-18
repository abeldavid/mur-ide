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

void AbstractConnection::initTimers()
{
    m_connectionTimeout->setInterval(m_disconnectInterval);
    m_updateDevicesTimer->setInterval(m_updateInterval);

    QObject::connect(m_connectionTimeout, SIGNAL(timeout()), this, SLOT(onDisconected()));
    QObject::connect(m_updateDevicesTimer, SIGNAL(timeout()), this, SLOT(updateRobotInfo()));

    m_updateDevicesTimer->start();
    m_connectionTimeout->start();
}
