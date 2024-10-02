#include "Global.h"
#include "Settings.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qSetMessagePattern(QStringLiteral("DateTime:%{time [yyyy-MM-dd hh:mm:ss:zzz]} Function:%{function} Message:%{message}\nFile:%{file} Line:%{line} ThreadId:%{threadid}"));

    QDir().mkpath(QCoreApplication::applicationDirPath()
                      .append(QStringLiteral("/debug")));

    Global::initOnce();

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
