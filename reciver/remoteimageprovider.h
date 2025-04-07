#ifndef REMOTEIMAGEPROVIDER_H
#define REMOTEIMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>
#include <QQuickPaintedItem>
#include <qwebsocket.h>

class RemoteImageProvider : public QQuickImageProvider
{
    Q_OBJECT
public:
    static constexpr const char *description = "remoteimage";

    RemoteImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    void addRemote(const QString &code, QWebSocket *socket);
    void deleteRemote(const QString &code);

private slots:
    void onVideoReceived(const QByteArray &message);

private:
    QHash<QWebSocket *, QString> wsList;
    QHash<QString, QImage> rooms;
};

#endif // REMOTEIMAGEPROVIDER_H
