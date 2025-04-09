#include "streamer.h"

AppStreamer::AppStreamer(QObject *parent)
    : QObject(parent)
    , eventManager({QEvent::MouseMove})
    , fpsRater(&imageSocket)
{
    connect(&fpsRater, &FpsRater::sendFrame, this, &AppStreamer::captureAndSendScreen);

    connect(&imageSocket, &QWebSocket::connected, this, &AppStreamer::onConnectedVideo);
    connect(&imageSocket, &QWebSocket::disconnected, this, &AppStreamer::onDisconnectedVideo);
    connect(&imageSocket, &QWebSocket::errorOccurred, this, &AppStreamer::onError);

    connect(&eventSocket, &QWebSocket::connected, this, &AppStreamer::onConnectedEvent);
    connect(&eventSocket, &QWebSocket::disconnected, this, &AppStreamer::onDisconnectedEvent);
    connect(&eventSocket, &QWebSocket::errorOccurred, this, &AppStreamer::onError);
    connect(&eventSocket, &QWebSocket::binaryMessageReceived, this, &AppStreamer::eventRecived);

    connect(&eventManager, &EventManager::newEvent, this, &AppStreamer::sendEvent);
    connect(&eventManager, &EventManager::serverEvent, this, &AppStreamer::serverEvent);
    connect(&eventManager, &EventManager::remoteMouseMove, this, &AppStreamer::remoteMouseMove);
}

void AppStreamer::setQmlEngine(QQmlApplicationEngine *engine)
{
    if (!engine->rootObjects().isEmpty()) {
        QObject *rootObject = engine->rootObjects().first();
        window = qobject_cast<QQuickWindow *>(rootObject);
        eventManager.setQQuickWindow(window);
    }
}

EventManager *AppStreamer::eventFilter()
{
    return &eventManager;
}

void AppStreamer::onConnectedVideo()
{
    fpsRater.start();
    qDebug() << "Connected to server";
}

void AppStreamer::onDisconnectedVideo()
{
    qDebug() << "Disconnected video from server";
}

void AppStreamer::onConnectedEvent()
{
    isConnected = true;
    emit isConnectedChanged();
}

void AppStreamer::onDisconnectedEvent()
{
    isConnected = false;
    emit isConnectedChanged();
    qDebug() << "Disconnected event from server";
}

void AppStreamer::onError(QAbstractSocket::SocketError error)
{
    fpsRater.stop();
    if (imageSocket.isValid()) {
        imageSocket.close();
    }
    if (eventSocket.isValid()) {
        eventSocket.close();
    }
    qWarning() << "WebSocket error:" << error;
}

void AppStreamer::captureAndSendScreen(int quality)
{
    if (!window)
        return;

    QImage image = window->grabWindow();

    QByteArray buffer;
    QBuffer b(&buffer);
    b.open(QIODevice::WriteOnly);
    image.save(&b, "JPEG", quality);

    if (imageSocket.isValid()) {
        imageSocket.sendBinaryMessage(b.buffer());
    }
}

void AppStreamer::eventRecived(const QByteArray &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonObject object = doc.object();
    eventManager.handleRemoteEvent(object);
}

void AppStreamer::sendEvent(QEvent *event)
{
    if (eventSocket.isValid()) {
        if (event->type() != QEvent::KeyPress || event->type() != QEvent::KeyRelease) {
            QJsonObject object = EventFactory::serialize(event, window->size());
            eventSocket.sendBinaryMessage(QJsonDocument{object}.toJson());
        }
    }
}

void AppStreamer::serverEvent(EventFactory::EventSource e)
{
    switch (e) {
    case EventFactory::FirstVideoReceiverConnected:
        emit videoReceiverConnected();
        return;
    case EventFactory::NewVideoReceiverConnected:
    case EventFactory::LastVideoReceiverDisconnected:
    case EventFactory::EventReceiverConnected:
        emit eventReceiverConnected();
        return;
    case EventFactory::EventReceiverDisconnected:
    case EventFactory::MouseEvent:
    case EventFactory::KeyboardEvent:
    case EventFactory::WheelEvent:
    case EventFactory::MouseClick:
    case EventFactory::KeyClick:
    case EventFactory::UnknowEvent:
        break;
    }
}

void AppStreamer::start()
{
    QNetworkRequest req(QUrl("http://localhost:3000/rooms"));
    req.setRawHeader("Authorization", "Bearer " + apiToken.toLocal8Bit());

    QNetworkReply *reply = manager.post(req, QByteArray{});

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->error() << reply->errorString();
            delete reply;
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

        QJsonObject data = doc.object();
        QString videoEndpoint = data.value("ws_video").toString();
        QString eventEndpoint = data.value("ws_events").toString();

        QNetworkRequest videoRequest(QUrl(videoEndpoint + "/sender"));
        videoRequest.setRawHeader("Authorization", "Bearer " + apiToken.toLocal8Bit());
        imageSocket.open(videoRequest);

        QNetworkRequest eventRequest(QUrl(eventEndpoint + "/sender"));
        eventRequest.setRawHeader("Authorization", "Bearer " + apiToken.toLocal8Bit());
        eventSocket.open(eventRequest);

        emit waitConnect(data.value("code").toString());
    });
}

void AppStreamer::stop()
{
    fpsRater.stop();
    imageSocket.close();
    eventSocket.close();
}
