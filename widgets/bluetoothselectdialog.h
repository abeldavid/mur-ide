#ifndef BLUETOOTHSELECTDIALOG_H
#define BLUETOOTHSELECTDIALOG_H

#include <QDialog>
#include <QString>
#include <QListWidget>

class BluetoothSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BluetoothSelectDialog(QWidget *parent = 0);

public slots:
    void showDeviceSelect(QByteArray devices);

signals:
    void deviceSelected(QString);

private:
    QListWidget* m_deviceList;
};

#endif // BLUETOOTHSELECTDIALOG_H
