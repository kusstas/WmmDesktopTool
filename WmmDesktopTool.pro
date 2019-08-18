QT += quick serialport quickcontrols2
CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        src/Device.cpp \
        src/LogInfoProxy.cpp \
        src/QmlFacade.cpp \
        src/main.cpp

RESOURCES += src/qml/qml.qrc

QML_IMPORT_PATH =
QML_DESIGNER_IMPORT_PATH =

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/Device.h \
    src/LogInfoProxy.h \
    src/QmlFacade.h
