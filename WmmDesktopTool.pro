QT += quick serialport quickcontrols2

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        Device.cpp \
        DeviceQmlProxies.cpp \
        DevicesMonitor.cpp \
        main.cpp

RESOURCES += qml.qrc

QML_IMPORT_PATH =
QML_DESIGNER_IMPORT_PATH =

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Device.h \
    DeviceCommon.h \
    DeviceQmlProxies.h \
    DevicesMonitor.h
