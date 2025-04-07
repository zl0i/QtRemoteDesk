#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "remoteimageprovider.h"
#include "streamer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    VideoReceiver *reciver = new VideoReceiver();

    app.installEventFilter(reciver->eventFilter());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("reciver", reciver);

    engine.addImageProvider(RemoteImageProvider::description, reciver->imageProvider());

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("FPDeskReciver", "Main");
    reciver->setQmlEngine(&engine);

    return app.exec();
}
