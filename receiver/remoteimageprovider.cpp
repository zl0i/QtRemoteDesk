#include "remoteimageprovider.h"

RemoteImageProvider::RemoteImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{}

QImage RemoteImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString code = id;
    if (rooms.contains(code)) {
        return QImage{rooms.value(code)};
    }
    return QImage{};
}

void RemoteImageProvider::addRemote(const QString &code, QWebSocket *socket)
{
    wsList.insert(socket, code);
    connect(socket, &QWebSocket::binaryFrameReceived, this, &RemoteImageProvider::onVideoReceived);
}

void RemoteImageProvider::deleteRemote(const QString &code)
{
    auto ws = wsList.key(code);
    disconnect(ws, &QWebSocket::binaryFrameReceived, this, &RemoteImageProvider::onVideoReceived);
    wsList.remove(ws);
}

void RemoteImageProvider::onVideoReceived(const QByteArray &message)
{
    auto ws = static_cast<QWebSocket *>(QObject::sender());
    QString code = wsList.value(ws);
    rooms.insert(code, QImage::fromData(message));
}
