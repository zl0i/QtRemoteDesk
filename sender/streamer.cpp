#include "streamer.h"

StreamServer::StreamServer(QObject *parent)
    : QObject(parent)
    , mouseEvent({QEvent::MouseMove})
    , fpsRater(&imageSocket)
{
    connect(&fpsRater, &FpsRater::sendFrame, this, &StreamServer::captureAndSendScreen);

    connect(&imageSocket, &QWebSocket::connected, this, &StreamServer::onConnectedVideo);
    connect(&imageSocket, &QWebSocket::disconnected, this, &StreamServer::onDisconnectedVideo);
    connect(&imageSocket, &QWebSocket::errorOccurred, this, &StreamServer::onError);

    connect(&eventSocket, &QWebSocket::connected, this, &StreamServer::onConnectedEvent);
    connect(&eventSocket, &QWebSocket::disconnected, this, &StreamServer::onDisconnectedEvent);
    connect(&eventSocket, &QWebSocket::errorOccurred, this, &StreamServer::onError);
    connect(&eventSocket, &QWebSocket::binaryMessageReceived, this, &StreamServer::eventRecived);

    connect(&mouseEvent, &MouseEventFilter::newEvent, this, &StreamServer::sendEvent);
}

void StreamServer::setQmlEngine(QQmlApplicationEngine *engine)
{
    if (!engine->rootObjects().isEmpty()) {
        QObject *rootObject = engine->rootObjects().first();
        window = qobject_cast<QQuickWindow *>(rootObject);
    }
}

MouseEventFilter *StreamServer::eventFilter()
{
    return &mouseEvent;
}

void StreamServer::onConnectedVideo()
{
    fpsRater.start();
    qDebug() << "Connected to server";
}

void StreamServer::onDisconnectedVideo()
{
    qDebug() << "Disconnected video from server";
}

void StreamServer::onConnectedEvent()
{
    isConnected = true;
    emit isConnectedChanged();
}

void StreamServer::onDisconnectedEvent()
{
    isConnected = false;
    emit isConnectedChanged();
    qDebug() << "Disconnected event from server";
}

void StreamServer::onError(QAbstractSocket::SocketError error)
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

void StreamServer::captureAndSendScreen(int quality)
{
    // QScreen *screen = QGuiApplication::primaryScreen();
    if (!window)
        return;

    // QPixmap pixmap = screen->grabWindow(0);
    // QImage image = //pixmap.toImage().scaled(800, 600, Qt::KeepAspectRatio);

    QImage image = window->grabWindow();

    QByteArray buffer;
    QBuffer b(&buffer);
    b.open(QIODevice::WriteOnly);
    image.save(&b, "JPEG", quality);

    if (imageSocket.isValid()) {
        imageSocket.sendBinaryMessage(b.buffer());
    }
}

void StreamServer::eventRecived(const QByteArray &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonObject object = doc.object();

    auto source = EventFactory::sourceEvent(object);
    if (source == EventFactory::MouseEvent) {
        auto *event = EventFactory::deserializeMouseEvent(object, window);
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *move = static_cast<QMouseEvent *>(event);
            emit remoteMouseMove(move->pos());
        }
        QCoreApplication::postEvent(window, event);
    } else if (source == EventFactory::KeyboardEvent) {
        auto *event = EventFactory::deserializeKeyboardEvent(object);
        QCoreApplication::postEvent(window, event);
    } else if (source == EventFactory::MouseClick) {
        auto events = EventFactory::deserializeMouseClick(object, window);
        QCoreApplication::postEvent(window, events.first);
        QCoreApplication::postEvent(window, events.second);
    } else if (source == EventFactory::KeyClick) {
        auto events = EventFactory::deserializeKeyboardClick(object);
        QCoreApplication::postEvent(window, events.first);
        QCoreApplication::postEvent(window, events.second);
    } else if (source == EventFactory::WheelEvent) {
        QWheelEvent *event = EventFactory::deserializeWheelEvent(object, window);
        QCoreApplication::postEvent(window, event);
    } else if (source == EventFactory::FirstVideoReceiverConnected) {
        emit videoReceiverConnected();
    } else if (source == EventFactory::EventReceiverConnected) {
        emit eventReceiverConnected();
    }
}

void StreamServer::sendEvent(QEvent *event)
{
    if (eventSocket.isValid()) {
        if (event->type() != QEvent::KeyPress || event->type() != QEvent::KeyRelease) {
            QJsonObject object = EventFactory::serialize(event, window->size());
            eventSocket.sendBinaryMessage(QJsonDocument{object}.toJson());
        }
    }
}

void StreamServer::start()
{
    QNetworkRequest req(QUrl("http://localhost:3000/rooms"));

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

        imageSocket.open(QUrl(videoEndpoint + "/sender"));
        eventSocket.open(QUrl(eventEndpoint + "/sender"));

        emit waitConnect(data.value("code").toString());
    });
}

void StreamServer::stop()
{
    fpsRater.stop();
    imageSocket.close();
    eventSocket.close();
}
