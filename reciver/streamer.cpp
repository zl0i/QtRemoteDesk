#include "streamer.h"

VideoReceiver::VideoReceiver(QObject *parent)
    : QObject(parent)
    , mouseEvent({QEvent::MouseButtonPress,
                  QEvent::MouseButtonRelease,
                  QEvent::MouseMove,
                  QEvent::MouseButtonDblClick,
                  QEvent::KeyPress,
                  QEvent::KeyRelease,
                  QEvent::Wheel})
{
    connect(&imageSocket, &QWebSocket::connected, this, &VideoReceiver::onConnectedVideo);
    connect(&imageSocket, &QWebSocket::disconnected, this, &VideoReceiver::onDisconnectedVideo);
    connect(&imageSocket, &QWebSocket::binaryMessageReceived, this, &VideoReceiver::onVideoReceived);

    connect(&eventSocket, &QWebSocket::connected, this, &VideoReceiver::onConnectedEvent);
    connect(&eventSocket, &QWebSocket::disconnected, this, &VideoReceiver::onDisconnectedEvent);
    connect(&eventSocket, &QWebSocket::binaryMessageReceived, this, &VideoReceiver::onEventReceived);

    connect(&mouseEvent, &MouseEventFilter::newEvent, this, &VideoReceiver::sendEvent);
}

void VideoReceiver::setQmlEngine(QQmlApplicationEngine *engine)
{
    if (!engine->rootObjects().isEmpty()) {
        QObject *rootObject = engine->rootObjects().first();
        window = qobject_cast<QQuickWindow *>(rootObject);
    }
}

void VideoReceiver::connectVideo(QString code)
{
    QNetworkRequest req(QUrl("http://localhost:3000/rooms/" + code));

    QNetworkReply *reply = manager.get(req);

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

        imageSocket.open(QUrl(videoEndpoint + "/receiver"));
        eventUrl = QUrl(eventEndpoint + "/receiver");
    });
}

void VideoReceiver::disconnectFromServer()
{
    imageSocket.close();
    eventSocket.close();
    eventUrl.clear();
}

QByteArray VideoReceiver::videoFrame() const
{
    return m_currentFrame;
}

void VideoReceiver::onVideoReceived(const QByteArray &message)
{
    m_currentFrame = message;
    emit frameUpdated();
}

void VideoReceiver::onEventReceived(const QByteArray &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QPointF point = EventSerializer::deserializeOnlyMouseMove(doc.object(), window->size());
    if (!point.isNull()) {
        emit mouseMove(point);
    }
}

void VideoReceiver::sendEvent(QEvent *event)
{
    if (eventSocket.isValid()) {
        QJsonObject object = EventSerializer::serialize(event, window->size());
        eventSocket.sendBinaryMessage(QJsonDocument{object}.toJson());
    }
}

void VideoReceiver::onConnectedVideo()
{
    m_isConnected = true;
    emit connectionChanged();
}

void VideoReceiver::onDisconnectedVideo()
{
    m_isConnected = false;
    emit connectionChanged();
}

void VideoReceiver::onConnectedEvent() {}

void VideoReceiver::onDisconnectedEvent() {}

void VideoReceiver::connectEvent()
{
    eventSocket.open(eventUrl);
}
