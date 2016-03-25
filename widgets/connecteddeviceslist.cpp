#include "connecteddeviceslist.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QLayout>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QFile>
#include <QSpacerItem>

/*
QLabel* m_thruster10;
QLabel* m_thruster20;
QLabel* m_thruster30;
QLabel* m_thruster40;

QLabel* m_emptySlot;
QLabel* m_altimetr;

QLabel* m_cameraOne;
QLabel* m_cameraTwo;

QLabel* m_depth;
QLabel* m_pitch;
QLabel* m_roll;
QLabel* m_yaw;
*/

ConnectedDevicesList::ConnectedDevicesList(QWidget *parent)
    : QWidget(parent),
      m_thruster10(new QLabel(this)),
      m_thruster20(new QLabel(this)),
      m_thruster30(new QLabel(this)),
      m_thruster40(new QLabel(this)),
      m_emptySlot(new QLabel(this)),
      m_altimetr(new QLabel(this)),
      m_cameraOne(new QLabel(this)),
      m_cameraTwo(new QLabel(this)),
      m_depth(new QLabel("90", this)),
      m_pitch(new QLabel("90", this)),
      m_roll(new QLabel("90", this)),
      m_yaw(new QLabel("90", this))
{
    setMaximumSize(230, 1024);
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    m_depth->setAlignment(Qt::AlignHCenter);
    m_pitch->setAlignment(Qt::AlignHCenter);
    m_roll->setAlignment(Qt::AlignHCenter);
    m_yaw->setAlignment(Qt::AlignHCenter);

    createPixmapLabel(m_thruster10, ":/icons/icons/widgeticons/thruster_10.png");
    createPixmapLabel(m_thruster20, ":/icons/icons/widgeticons/thruster_20.png");
    createPixmapLabel(m_thruster30, ":/icons/icons/widgeticons/thruster_30.png");
    createPixmapLabel(m_thruster40, ":/icons/icons/widgeticons/thruster_40.png");

    createPixmapLabel(m_emptySlot, ":/icons/icons/widgeticons/empty_port.png");
    createPixmapLabel(m_altimetr, ":/icons/icons/widgeticons/empty_port.png");
    createPixmapLabel(m_cameraOne, ":/icons/icons/widgeticons/camera.png");
    createPixmapLabel(m_cameraTwo, ":/icons/icons/widgeticons/camera_dark.png");

    m_thruster10->setAlignment(Qt::AlignHCenter);
    m_thruster20->setAlignment(Qt::AlignHCenter);
    m_thruster30->setAlignment(Qt::AlignHCenter);
    m_thruster40->setAlignment(Qt::AlignHCenter);

    m_emptySlot->setAlignment(Qt::AlignHCenter);
    m_altimetr->setAlignment(Qt::AlignHCenter);
    m_cameraOne->setAlignment(Qt::AlignHCenter);
    m_cameraTwo->setAlignment(Qt::AlignHCenter);

    QLabel* headerYaw = new QLabel("Курс", this);
    QLabel* headerPitch = new QLabel("Крен", this);
    QLabel* headerRoll = new QLabel("Дифф.", this);
    QLabel* headerDepth = new QLabel("Глуб.", this);

    headerYaw->setAlignment(Qt::AlignHCenter);
    headerRoll->setAlignment(Qt::AlignHCenter);
    headerPitch->setAlignment(Qt::AlignHCenter);
    headerDepth->setAlignment(Qt::AlignHCenter);

    mainLayout->addWidget(headerYaw, 0, 0);
    mainLayout->addWidget(headerPitch, 0, 1);
    mainLayout->addWidget(headerRoll, 0, 2);
    mainLayout->addWidget(headerDepth, 0, 3);

    mainLayout->addWidget(m_yaw, 1, 0);
    mainLayout->addWidget(m_pitch, 1, 1);
    mainLayout->addWidget(m_roll, 1, 2);
    mainLayout->addWidget(m_depth, 1, 3);

    mainLayout->addWidget(m_thruster10, 2, 0);
    mainLayout->addWidget(m_thruster20, 2, 1);
    mainLayout->addWidget(m_thruster30, 2, 2);
    mainLayout->addWidget(m_thruster40, 2, 3);

    mainLayout->addWidget(m_cameraOne, 3, 0);
    mainLayout->addWidget(m_cameraTwo, 3, 1);
    mainLayout->addWidget(m_altimetr, 3, 2);
    mainLayout->addWidget(m_emptySlot, 3, 3);


    QFile mainWindowSheet(":/dark/styles/devicelist.css");
    mainWindowSheet.open(QFile::ReadOnly);
    QString styleSheet = mainWindowSheet.readAll();
    setStyleSheet(styleSheet);
    mainWindowSheet.close();

    mainLayout->setSpacing(10);
    /*
    mainLayout->addWidget(createHeaderLabel("Порт 1"), 0, 0);
    mainLayout->addWidget(createPixmapLabel(), 1, 0);
    mainLayout->addWidget(createHeaderLabel("Порт 2"), 0, 1);
    mainLayout->addWidget(createPixmapLabel(), 1, 1);
    mainLayout->addWidget(createHeaderLabel("Порт 3"), 0, 2);
    mainLayout->addWidget(createPixmapLabel(), 1, 2);
    mainLayout->addWidget(createHeaderLabel("Порт 4"), 0, 3);
    mainLayout->addWidget(createPixmapLabel(), 1, 3);

    mainLayout->addWidget(createHeaderLabel("Порт 5"), 2, 0);
    mainLayout->addWidget(createPixmapLabel(), 3, 0);
    mainLayout->addWidget(createHeaderLabel("Порт 6"), 2, 1);
    mainLayout->addWidget(createPixmapLabel(), 3, 1);
    mainLayout->addWidget(createHeaderLabel("Порт 7"), 2, 2);
    mainLayout->addWidget(createPixmapLabel(), 3, 2);
    mainLayout->addWidget(createHeaderLabel("Порт 8"), 2, 3);
    mainLayout->addWidget(createPixmapLabel(), 3, 3);
    */
}

QLabel *ConnectedDevicesList::createHeaderLabel(const QString &text)
{
    QLabel *label = new QLabel(text, this);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

void ConnectedDevicesList::createPixmapLabel(QLabel *label, const QString &icon)
{
    label->setPixmap(QPixmap(icon));
}


