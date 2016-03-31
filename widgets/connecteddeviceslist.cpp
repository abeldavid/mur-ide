#include "connecteddeviceslist.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QLayout>
#include <QLabel>
#include <QFile>
#include <QSpacerItem>
#include <QDebug>

ConnectedDevicesList::ConnectedDevicesList(QWidget *parent)
    : QWidget(parent),
      m_thruster10(new QLabel(this)),
      m_thruster20(new QLabel(this)),
      m_thruster30(new QLabel(this)),
      m_thruster40(new QLabel(this)),
      m_cameraOne(new QLabel(this)),
      m_cameraTwo(new QLabel(this)),
      m_altimetr(new QLabel(this)),
      m_emptySlot(new QLabel(this)),
      m_yaw(new QLabel("", this)),
      m_pitch(new QLabel("", this)),
      m_roll(new QLabel("", this)),
      m_depth(new QLabel("", this)),
      m_prevStatusInfo({})
{
    setMaximumSize(230, 1024);
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    QLabel* headerYaw = new QLabel("Курс", this);
    headerYaw->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(headerYaw, 0, 0);
    QLabel* headerPitch = new QLabel("Крен", this);
    headerPitch->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(headerPitch, 0, 1);
    QLabel* headerRoll = new QLabel("Дифф.", this);
    headerRoll->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(headerRoll, 0, 2);
    QLabel* headerDepth = new QLabel("Глуб.", this);
    headerDepth->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(headerDepth, 0, 3);

    m_depth->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_yaw, 1, 0);
    m_pitch->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_pitch, 1, 1);
    m_roll->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_roll, 1, 2);
    m_yaw->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_depth, 1, 3);

    m_thruster10Icons.first = QPixmap(":/icons/icons/widgeticons/thruster_10.png");
    m_thruster10Icons.second = QPixmap(":/icons/icons/widgeticons/thruster_10_dark.png");
    m_thruster20Icons.first = QPixmap(":/icons/icons/widgeticons/thruster_20.png");
    m_thruster20Icons.second = QPixmap(":/icons/icons/widgeticons/thruster_20_dark.png");
    m_thruster30Icons.first = QPixmap(":/icons/icons/widgeticons/thruster_30.png");
    m_thruster30Icons.second = QPixmap(":/icons/icons/widgeticons/thruster_30_dark.png");
    m_thruster40Icons.first = QPixmap(":/icons/icons/widgeticons/thruster_40.png");
    m_thruster40Icons.second = QPixmap(":/icons/icons/widgeticons/thruster_40_dark.png");

    m_cameraOneIcons.first = QPixmap(":/icons/icons/widgeticons/camera.png");
    m_cameraOneIcons.second = QPixmap(":/icons/icons/widgeticons/camera_dark.png");
    m_cameraTwoIcons.first = QPixmap(":/icons/icons/widgeticons/camera.png");
    m_cameraTwoIcons.second = QPixmap(":/icons/icons/widgeticons/camera_dark.png");
    m_altimetrIcons.first = QPixmap(":/icons/icons/widgeticons/empty_port.png");
    m_altimetrIcons.second = QPixmap(":/icons/icons/widgeticons/empty_port.png");
    m_emptySlotIcons.first = QPixmap(":/icons/icons/widgeticons/empty_port.png");
    m_emptySlotIcons.second = QPixmap(":/icons/icons/widgeticons/empty_port.png");


    m_thruster10->setPixmap(m_thruster10Icons.second);
    m_thruster10->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_thruster10, 2, 0);
    m_thruster20->setPixmap(m_thruster20Icons.second);
    m_thruster20->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_thruster20, 2, 1);
    m_thruster30->setPixmap(m_thruster30Icons.second);
    m_thruster30->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_thruster30, 2, 2);
    m_thruster40->setPixmap(m_thruster40Icons.second);
    m_thruster40->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_thruster40, 2, 3);

    m_cameraOne->setPixmap(m_cameraOneIcons.second);
    m_cameraOne->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_cameraOne, 3, 0);
    m_cameraTwo->setPixmap(m_cameraTwoIcons.second);
    m_cameraTwo->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_cameraTwo, 3, 1);
    m_altimetr->setPixmap(m_altimetrIcons.second);
    m_altimetr->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_altimetr, 3, 2);
    m_emptySlot->setPixmap(m_emptySlotIcons.second);
    m_emptySlot->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(m_emptySlot, 3, 3);

    QFile styleFile(":/dark/styles/devicelist.css");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = styleFile.readAll();
    setStyleSheet(styleSheet);
    styleFile.close();

    mainLayout->setSpacing(10);
}

void ConnectedDevicesList::updateDevices(const StatusInfo &status)
{
    updateDeviceIcon(0, status.devicesTypes[0], m_thruster10, m_thruster10Icons);
    updateDeviceIcon(1, status.devicesTypes[1], m_thruster20, m_thruster20Icons);
    updateDeviceIcon(2, status.devicesTypes[2], m_thruster30, m_thruster30Icons);
    updateDeviceIcon(3, status.devicesTypes[3], m_thruster40, m_thruster40Icons);
    updateDeviceIcon(4, status.devicesTypes[4], m_altimetr, m_altimetrIcons);
    updateDeviceIcon(5, status.devicesTypes[5], m_emptySlot, m_emptySlotIcons);

    if (m_prevStatusInfo.cameras != status.cameras) {
        switch (status.cameras) {
            case 0:
                m_cameraOne->setPixmap(m_cameraOneIcons.second);
                m_cameraTwo->setPixmap(m_cameraTwoIcons.second);
                break;
            case 1:
                m_cameraOne->setPixmap(m_cameraOneIcons.first);
                m_cameraTwo->setPixmap(m_cameraTwoIcons.second);
                break;
            case 2:
                m_cameraOne->setPixmap(m_cameraOneIcons.first);
                m_cameraTwo->setPixmap(m_cameraTwoIcons.first);
                break;
        }
    }

    if (m_prevStatusInfo.yaw != status.yaw) {
        m_yaw->setText(QString::number(status.yaw, 'f', 2));
    }
    if (m_prevStatusInfo.pitch != status.pitch) {
        m_pitch->setText(QString::number(status.pitch, 'f', 2));
    }
    if (m_prevStatusInfo.roll != status.roll) {
        m_roll->setText(QString::number(status.roll, 'f', 2));
    }
    if (m_prevStatusInfo.depth != status.depth) {
        m_depth->setText(QString::number(status.depth, 'f', 2));
    }

    m_prevStatusInfo = status;
}

void ConnectedDevicesList::updateDeviceIcon(uint8_t deviceNumber, const uint8_t &deviceStatus, QLabel *deviceLabel, const QPair<QPixmap, QPixmap> &icons)
{
    if (m_prevStatusInfo.devicesTypes[deviceNumber] != deviceStatus) {
        if (deviceStatus == 1) {
            deviceLabel->setPixmap(icons.first);
        } else if (deviceStatus == 0) {
            deviceLabel->setPixmap(icons.second);
        }
    }
}


