#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QLabel>
#include "bluetoothselectdialog.h"

BluetoothSelectDialog::BluetoothSelectDialog(QWidget *parent) :
    QDialog(parent)

{
    setMinimumWidth(300);
    setMinimumHeight(100);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Bluetooth девайсы");
    setLayout(new QVBoxLayout(this));
}

void BluetoothSelectDialog::showDeviceSelect(QByteArray devices)
{
    delete this->layout();
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel* bluetoothLabel = new QLabel("Устройства Bluetooth");
    layout->addWidget(bluetoothLabel);
    QFrame* myFrame = new QFrame();
    myFrame->setFrameShape(QFrame::HLine);
    layout->addWidget(myFrame);

    setLayout(layout);

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(devices, &err);
    if (err.error == QJsonParseError::NoError) {
        QJsonArray jsonDevices = jsonDoc.array();
        for (auto device: jsonDevices) {
            QJsonArray devArray(device.toArray());
            QLabel* bluetoothDeviceLabel = new QLabel(
                        devArray.at(0).toString() + " - " + devArray.at(1).toString(), this);
            layout->addWidget(bluetoothDeviceLabel);
        }
    }
}
