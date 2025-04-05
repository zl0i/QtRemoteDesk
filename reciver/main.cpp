#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "streamer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    VideoReceiver reciver;

    app.installEventFilter(reciver.eventFilter());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("reciver", &reciver);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("FPDeskReciver", "Main");

    return app.exec();
}
