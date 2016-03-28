#ifndef CONNECTEDDEVICESLIST_H
#define CONNECTEDDEVICESLIST_H

#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include "wificonnection.h"

class ConnectedDevicesList : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectedDevicesList(QWidget *parent = 0);

public slots:
    void updateDevices(const StatusInfo &status);
private:
    QLabel* m_thruster10;
    QPair <QPixmap, QPixmap> m_thruster10Icons;
    QLabel* m_thruster20;
    QPair <QPixmap, QPixmap> m_thruster20Icons;
    QLabel* m_thruster30;
    QPair <QPixmap, QPixmap> m_thruster30Icons;
    QLabel* m_thruster40;
    QPair <QPixmap, QPixmap> m_thruster40Icons;

    QLabel* m_cameraOne;
    QPair <QPixmap, QPixmap> m_cameraOneIcons;
    QLabel* m_cameraTwo;
    QPair <QPixmap, QPixmap> m_cameraTwoIcons;

    QLabel* m_altimetr;
    QPair <QPixmap, QPixmap> m_altimetrIcons;
    QLabel* m_emptySlot;
    QPair <QPixmap, QPixmap> m_emptySlotIcons;

    QLabel* m_yaw;
    QLabel* m_pitch;
    QLabel* m_roll;
    QLabel* m_depth;

    StatusInfo m_prevStatusInfo;
};

#endif // CONNECTEDDEVICESLIST_H
