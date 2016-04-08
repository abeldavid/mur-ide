#ifndef CONNECTEDDEVICESLIST_H
#define CONNECTEDDEVICESLIST_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include "wificonnection.h"

class ConnectedDevicesList : public QWidget
{
    Q_OBJECT
public:
    static const QString wifiCaption;
    static const QString bluetoothCaption;
    explicit ConnectedDevicesList(QWidget *parent = 0);
    ~ConnectedDevicesList();

public slots:
    void updateDevices(const StatusInfo &status);
    void updateDeviceIcon(uint8_t deviceNumber, const uint8_t &deviceStatus, QLabel *deviceLabel, const QPair<QPixmap, QPixmap> &icons);
    void clearDevices();

signals:
    void connectToWifi();
    void connectToBluetooth();

private:
    QGridLayout *m_mainLayout;
    QVBoxLayout *m_connectionLayout;
    QPair<QString, QString> m_connectionTexts;
    QComboBox* m_connectionType;
    QLabel* m_connectionStatus;
    QLabel* m_thruster10;
    QPair<QPixmap, QPixmap> m_thruster10Icons;
    QLabel* m_thruster20;
    QPair<QPixmap, QPixmap> m_thruster20Icons;
    QLabel* m_thruster30;
    QPair<QPixmap, QPixmap> m_thruster30Icons;
    QLabel* m_thruster40;
    QPair<QPixmap, QPixmap> m_thruster40Icons;

    QLabel* m_cameraOne;
    QPair<QPixmap, QPixmap> m_cameraOneIcons;
    QLabel* m_cameraTwo;
    QPair<QPixmap, QPixmap> m_cameraTwoIcons;

    QLabel* m_altimetr;
    QPair<QPixmap, QPixmap> m_altimetrIcons;
    QLabel* m_emptySlot;
    QPair<QPixmap, QPixmap> m_emptySlotIcons;

    QLabel* m_headerYaw;
    QLabel* m_headerPitch;
    QLabel* m_headerRoll;
    QLabel* m_headerDepth;

    QLabel* m_yaw;
    QLabel* m_pitch;
    QLabel* m_roll;
    QLabel* m_depth;

    StatusInfo m_prevStatusInfo;
    void createPixMaps();
    void initWidgets();
private slots:
    void setConnectionType(QString type);
};

#endif // CONNECTEDDEVICESLIST_H
