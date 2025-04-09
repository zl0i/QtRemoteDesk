#include "videoreceiver.h"

VideoReceiver::VideoReceiver(QObject *parent)
    : QObject(parent)
    , eventManager({QEvent::MouseButtonPress,
                    QEvent::MouseButtonRelease,
                    QEvent::MouseMove,
                    QEvent::MouseButtonDblClick,
                    QEvent::KeyPress,
                    QEvent::KeyRelease,
                    QEvent::Wheel},
                   parent)
    , imageProivder(new RemoteImageProvider())
{
    connect(&imageSocket, &QWebSocket::connected, this, &VideoReceiver::onConnectedVideo);
    connect(&imageSocket, &QWebSocket::disconnected, this, &VideoReceiver::onDisconnectedVideo);
    connect(&imageSocket, &QWebSocket::binaryMessageReceived, this, &VideoReceiver::frameUpdated);

    connect(&eventSocket, &QWebSocket::connected, this, &VideoReceiver::onConnectedEvent);
    connect(&eventSocket, &QWebSocket::disconnected, this, &VideoReceiver::onDisconnectedEvent);
    connect(&eventSocket, &QWebSocket::binaryMessageReceived, this, &VideoReceiver::onEventReceived);

    connect(&eventManager, &EventManager::newEvent, this, &VideoReceiver::sendEvent);
}

VideoReceiver::~VideoReceiver()
{
    imageProivder->deleteLater();
}

void VideoReceiver::setQmlEngine(QQmlApplicationEngine *engine)
{
    if (!engine->rootObjects().isEmpty()) {
        QObject *rootObject = engine->rootObjects().first();
        window = qobject_cast<QQuickWindow *>(rootObject);
        eventManager.setWindow(window);
    }
}

void VideoReceiver::connectVideo(QString code)
{
    QNetworkRequest req(QUrl("http://localhost:3000/rooms/" + code));
    req.setRawHeader("Authorization", "Bearer " + apiToken.toLocal8Bit());

    QNetworkReply *reply = manager.get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply, code]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->error() << reply->errorString();
            delete reply;
            return;
        }

        this->code = code;
        emit codeChanged();
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

        QJsonObject data = doc.object();
        QString videoEndpoint = data.value("ws_video").toString();
        QString eventEndpoint = data.value("ws_events").toString();

        QNetworkRequest videoRequest(QUrl(videoEndpoint + "/receiver"));
        videoRequest.setRawHeader("Authorization", "Bearer " + apiToken.toLocal8Bit());
        imageSocket.open(videoRequest);
        imageProivder->addRemote(code, &imageSocket);

        eventUrl = QUrl(eventEndpoint + "/receiver");
    });
}

void VideoReceiver::disconnectFromServer()
{
    imageSocket.close();
    eventSocket.close();
    eventUrl.clear();
    imageProivder->deleteRemote(code);
    code.clear();
}

void VideoReceiver::onEventReceived(const QByteArray &message)
{
    //hande server event
}

void VideoReceiver::sendEvent(QJsonObject object)
{
    if (eventSocket.isValid()) {
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
    QNetworkRequest eventRequest(eventUrl);
    eventRequest.setRawHeader("Authorization", "Bearer " + apiToken.toLocal8Bit());
    eventSocket.open(eventRequest);
}
