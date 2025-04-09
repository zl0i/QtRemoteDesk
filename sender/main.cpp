#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "streamer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AppStreamer streamer(&engine);
    app.installEventFilter(streamer.eventFilter());
    engine.rootContext()->setContextProperty("streamer", &streamer);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("FPDeskSender", "Main");

    streamer.setQmlEngine(&engine);

    return app.exec();
}
