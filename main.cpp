#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "Device.h"
#include "DevicesMonitor.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Material");

    QGuiApplication app(argc, argv);
    DevicesMonitor deviceMonitor;
    Device device;

    auto updatePortName = [&]{
        if (deviceMonitor.selectedIndex() >= 0)
        {
            device.setPortName(deviceMonitor.names()[deviceMonitor.selectedIndex()]);
        }
        else
        {
            device.setPortName("");
        }
    };

    updatePortName();
    QObject::connect(&deviceMonitor, &DevicesMonitor::selectedIndexChanged, &device, updatePortName);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("deviceMonitor", &deviceMonitor);
    engine.rootContext()->setContextProperty("device", &device);
    QUrl const url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, QUrl const& objUrl) {
        if (!obj && url == objUrl)
        {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
