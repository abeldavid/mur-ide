#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include "bluetoothselectdialog.h"

BluetoothSelectDialog::BluetoothSelectDialog(QWidget *parent) :
    QDialog(parent)
{
    setMinimumWidth(300);
    setMinimumHeight(100);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Bluetooth девайсы");

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel* bluetoothLabel = new QLabel("Устройства Bluetooth");
    layout->addWidget(bluetoothLabel);
    m_deviceList = new QListWidget(this);
    layout->addWidget(m_deviceList);
    setLayout(layout);
    connect(m_deviceList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(listItemSelected(QListWidgetItem*)));
}

void BluetoothSelectDialog::showDeviceSelect(QByteArray devices)
{
    m_deviceList->clear();
    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(devices, &err);
    if (err.error == QJsonParseError::NoError) {
        QJsonArray jsonDevices = jsonDoc.array();
        for (auto device: jsonDevices) {
            QJsonArray devArray(device.toArray());
            m_deviceList->addItem(devArray.at(0).toString() + m_nameSeparator + devArray.at(1).toString());
        }
    }
}

void BluetoothSelectDialog::listItemSelected(QListWidgetItem* item)
{
    emit deviceSelected(item->text().split(m_nameSeparator)[0]);
    close();
}
