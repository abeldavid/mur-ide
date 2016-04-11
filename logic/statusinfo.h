#ifndef STATUSINFO_H
#define STATUSINFO_H

#include <QObject>

static const std::size_t DEVICES_NUM = 6;

struct StatusInfo
{
    uint8_t devicesTypes[DEVICES_NUM];
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float depth = 0.0f;
    uint8_t leak = 0;
    uint8_t version = 0;
    uint8_t cameras = 0;
};

Q_DECLARE_METATYPE(StatusInfo)

#endif // STATUSINFO_H
