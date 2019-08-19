#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QDir>

#include "QmlFacade.h"
#include "Device.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("WmmDesktopTool");
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

    QStringList docDirs = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    QStringList downDirs = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    if (!docDirs.isEmpty())
    {
        QDir docDir(docDirs.first());
        docDir.mkdir(QCoreApplication::applicationName());
        engine.rootContext()->setContextProperty("defaultLogDir",
                                                 docDir.filePath(QCoreApplication::applicationName()));
    }
    else if (!downDirs.isEmpty())
    {
        QDir downDir(downDirs.first());
        downDir.mkdir(QCoreApplication::applicationName());
        engine.rootContext()->setContextProperty("defaultLogDir",
                                                 downDir.filePath(QCoreApplication::applicationName()));
    }
    else
    {
        engine.rootContext()->setContextProperty("defaultLogDir", "");
    }

    engine.rootContext()->setContextProperty("facade", &facade);
    engine.load(url);

    return app.exec();
}
