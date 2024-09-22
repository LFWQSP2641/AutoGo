#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QDir().mkpath(QCoreApplication::applicationDirPath()
                      .append(QStringLiteral("/debug")));

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
