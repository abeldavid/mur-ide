#ifndef BLUETOOTHCONNECTION_H
#define BLUETOOTHCONNECTION_H

#include <QObject>
#include "abstractconnection.h"

class BluetoothConnection : public AbstractConnection
{
    Q_OBJECT
public:
    explicit BluetoothConnection(QObject *parent = 0);

signals:

public slots:
};

#endif // BLUETOOTHCONNECTION_H
