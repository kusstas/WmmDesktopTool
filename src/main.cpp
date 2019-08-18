#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "QmlFacade.h"
#include "Device.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Wmm Desktop Tool");
    QCoreApplication::setOrganizationName("Svarmed");
    QCoreApplication::setOrganizationDomain("com.svarmed.wmm");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Material");

    qmlRegisterType<Device>("com.svarmed.backend", 1, 0, "Device");

    QGuiApplication app(argc, argv);
    QmlFacade facade;

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl)
        {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("defaultLogDir",
                                             QGuiApplication::applicationDirPath());
    engine.rootContext()->setContextProperty("facade", &facade);
    engine.load(url);

    return app.exec();
}
