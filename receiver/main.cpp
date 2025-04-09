#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "remoteimageprovider.h"
#include "videoreceiver.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    VideoReceiver *receiver = new VideoReceiver();
    app.installEventFilter(receiver->eventFilter());
    engine.rootContext()->setContextProperty("receiver", receiver);
    engine.addImageProvider(RemoteImageProvider::description, receiver->imageProvider());

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("FPDeskReceiver", "Main");
    receiver->setQmlEngine(&engine);

    return app.exec();
}
