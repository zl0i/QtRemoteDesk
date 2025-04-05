#include "streamer.h"

StreamServer::StreamServer(QObject *parent)
    : QObject(parent)
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

    connect(&mouseEvent, &MouseEventFilter::mouseClicked, this, &StreamServer::sendClickMouseEvent);
    connect(&mouseEvent, &MouseEventFilter::mouseMoved, this, &StreamServer::sendMovedMouseEvent);
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
    // imageSocket.sendTextMessage("sender"); // Идентифицируем как отправитель
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
    QStringList list = QString{message}.split(";");

    if (list.count() < 2)
        return;

    QString type = list.at(0);
    int x = list.at(1).toInt();
    int y = list.at(2).toInt();
    if (list.at(0) == "click") {
        QPointF pos(x, y);
        QPoint globalPos = window->mapToGlobal(QPoint(x, y));

        QMouseEvent press(QEvent::MouseButtonPress,
                          pos,
                          globalPos,
                          Qt::LeftButton,
                          Qt::LeftButton,
                          Qt::NoModifier);
        QCoreApplication::sendEvent(window, &press);

        QMouseEvent release(QEvent::MouseButtonRelease,
                            pos,
                            globalPos,
                            Qt::LeftButton,
                            Qt::LeftButton,
                            Qt::NoModifier);
        QCoreApplication::sendEvent(window, &release);

    } else if (list.at(0) == "move") {
        emit remoteMouseMove({x, y});
    }
}

void StreamServer::sendMovedMouseEvent(QPoint point)
{
    if (eventSocket.isValid()) {
        eventSocket.sendTextMessage(QString{"move;%1;%2"}.arg(point.x()).arg(point.y()));
    }
}

void StreamServer::sendClickMouseEvent(QPoint point)
{
    if (eventSocket.isValid()) {
        eventSocket.sendTextMessage(QString{"click;%1;%2"}.arg(point.x()).arg(point.y()));
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
