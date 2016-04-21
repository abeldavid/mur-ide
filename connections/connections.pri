INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/abstractconnection.h \
    $$PWD/wificonnection.h \
    $$PWD/bluetoothconnection.h \
    $$PWD/connectionmanager.h \
    $$PWD/usbconnection.h \
    $$PWD/protocol/RoboPotocol.pb.h

SOURCES += \
    $$PWD/abstractconnection.cpp \
    $$PWD/wificonnection.cpp \
    $$PWD/bluetoothconnection.cpp \
    $$PWD/connectionmanager.cpp \
    $$PWD/usbconnection.cpp \
    $$PWD/protocol/RoboPotocol.pb.cc

