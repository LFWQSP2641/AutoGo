#include "Global.h"
#include "Settings.h"

#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qSetMessagePattern(QStringLiteral("DateTime:%{time [yyyy-MM-dd hh:mm:ss:zzz]} Function:%{function} Message:%{message}\nFile:%{file} Line:%{line} ThreadId:%{threadid}"));

    Global::initOnce();

    QQuickStyle::setStyle(QStringLiteral("Material"));

    qmlRegisterSingletonInstance("Settings", 1, 0, "Settings", Settings::getSingletonSettings());

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/ui/qml/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
