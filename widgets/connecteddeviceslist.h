#ifndef CONNECTEDDEVICESLIST_H
#define CONNECTEDDEVICESLIST_H

#include <QWidget>
#include <QLabel>
#include <QIcon>

class ConnectedDevicesList : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectedDevicesList(QWidget *parent = 0);

private:
    QLabel *createHeaderLabel(const QString &text);
    void createPixmapLabel(QLabel *label, const QString &icon);
public slots:
private:
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
};

#endif // CONNECTEDDEVICESLIST_H
