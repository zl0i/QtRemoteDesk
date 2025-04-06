#include "streamer.h"

StreamServer::StreamServer(QObject *parent)
    : QObject(parent)
    , mouseEvent({QEvent::MouseMove})
{
    timer.setInterval(1000 / 10); // 10 FPS
    connect(&timer, &QTimer::timeout, this, &StreamServer::broadcastFrame);

    connect(&imageSocket, &QWebSocket::connected, this, &StreamServer::onConnectedVideo);
    connect(&imageSocket, &QWebSocket::disconnected, this, &StreamServer::onDisconnectedVideo);
    connect(&imageSocket,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this,
            &StreamServer::onError);

    connect(&eventSocket, &QWebSocket::connected, this, &StreamServer::onConnectedEvent);
    connect(&eventSocket, &QWebSocket::disconnected, this, &StreamServer::onDisconnectedEvent);
    connect(&eventSocket,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this,
            &StreamServer::onError);
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
    timer.start();
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
    timer.stop();
    if (imageSocket.isValid()) {
        imageSocket.close();
    }
    if (eventSocket.isValid()) {
        eventSocket.close();
    }
    qWarning() << "WebSocket error:" << error;
}

void StreamServer::broadcastFrame()
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
    image.save(&b, "JPEG");

    QByteArray base64 = buffer.toBase64();
    QString dataUrl = "data:image/jpeg;base64," + QString::fromLatin1(base64);

    if (imageSocket.isValid()) {
        imageSocket.sendTextMessage(dataUrl.toLocal8Bit());
    }
}

void StreamServer::eventRecived(const QByteArray &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    auto source = EventFactory::sourceEvent(doc.object());
    if (source == EventFactory::MouseEvent) {
        QMouseEvent event = EventFactory::deserializeMouseEvent(doc.object(), window);
        if (event.type() == QEvent::MouseMove) {
            emit remoteMouseMove(event.pos());
        } else {
            QCoreApplication::sendEvent(window, &event);
        }
    } else if (source == EventFactory::KeyboardEvent) {
        QKeyEvent event = EventFactory::deserializeKeyboardEvent(doc.object());
        QCoreApplication::sendEvent(window, &event);
    }
}

void StreamServer::sendEvent(QEvent *event)
{
    if (eventSocket.isValid()) {
        QJsonObject object = EventFactory::serialize(event);
        eventSocket.sendBinaryMessage(QJsonDocument{object}.toJson());
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
    timer.stop();
    imageSocket.close();
    eventSocket.close();
}
