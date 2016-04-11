#include "connectionmanager.h"
#include "wificonnection.h"
#include "bluetoothconnection.h"

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent)
{
    m_connection = new WiFiConnection()
}

