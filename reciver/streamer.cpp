#include "streamer.h"

VideoReceiver::VideoReceiver(QObject *parent)
    : QObject(parent)
{
    connect(&imageSocket, &QWebSocket::connected, this, &VideoReceiver::onConnectedVideo);
    connect(&imageSocket, &QWebSocket::disconnected, this, &VideoReceiver::onDisconnectedVideo);
    connect(&imageSocket, &QWebSocket::binaryMessageReceived, this, &VideoReceiver::onVideoReceived);

    connect(&eventSocket, &QWebSocket::connected, this, &VideoReceiver::onConnectedEvent);
    connect(&eventSocket, &QWebSocket::disconnected, this, &VideoReceiver::onDisconnectedEvent);
    connect(&eventSocket, &QWebSocket::binaryMessageReceived, this, &VideoReceiver::onEventReceived);

    connect(&mouseEvent, &MouseEventFilter::mouseClicked, this, &VideoReceiver::sendClickMouseEvent);
    connect(&mouseEvent, &MouseEventFilter::mouseMoved, this, &VideoReceiver::sendMovedMouseEvent);
}

void VideoReceiver::connectToServer(QString code)
{
    QString url = QString{"ws://localhost:8080/rooms/%1"}.arg(code);
    imageSocket.open(QUrl(url + "/video/receiver"));
    eventSocket.open(QUrl(url + "/events/receiver"));
}

void VideoReceiver::disconnectFromServer()
{
    imageSocket.close();
    eventSocket.close();
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
    QStringList list = QString{message}.split(";");
    QString type = list.at(0);
    int x = list.at(1).toInt();
    int y = list.at(2).toInt();
    if (list.at(0) == "click") {
        emit mouseClick({x, y});
    } else if (list.at(0) == "move") {
        emit mouseMove({x, y});
    }
}

void VideoReceiver::sendMovedMouseEvent(QPoint point)
{
    if (eventSocket.isValid()) {
        eventSocket.sendTextMessage(QString{"move;%1;%2"}.arg(point.x()).arg(point.y()));
    }
}

void VideoReceiver::sendClickMouseEvent(QPoint point)
{
    if (eventSocket.isValid()) {
        eventSocket.sendTextMessage(QString{"click;%1;%2"}.arg(point.x()).arg(point.y()));
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
