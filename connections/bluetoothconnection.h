#ifndef BLUETOOTHCONNECTION_H
#define BLUETOOTHCONNECTION_H

#include <QObject>

class BluetoothConnection : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothConnection(QObject *parent = 0);

signals:

public slots:
};

#endif // BLUETOOTHCONNECTION_H