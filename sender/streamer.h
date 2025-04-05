#ifndef STREAMER_H
#define STREAMER_H

#include <QBuffer>
#include <QByteArray>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QQuickWindow>
#include <QScreen>
#include <QTimer>
#include <QWebSocket>
#include <QWebSocketServer>
#include <qqmlapplicationengine.h>

#include "mouseeventfilter.h"

class StreamServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ readIsConnected NOTIFY isConnectedChanged FINAL)
public:
    StreamServer(QObject *parent = nullptr);

    void setQmlEngine(QQmlApplicationEngine *engine);

    MouseEventFilter *eventFilter();

    bool readIsConnected() { return isConnected; }

private:
    QQuickWindow *window;
    QTimer timer;
    QWebSocket imageSocket;
    QWebSocket eventSocket;
    bool isConnected{false};

    MouseEventFilter mouseEvent;
    QNetworkAccessManager manager;

private slots:
    void onConnectedVideo();
    void onDisconnectedVideo();
    void onConnectedEvent();
    void onDisconnectedEvent();
    void onError(QAbstractSocket::SocketError error);
    void broadcastFrame();
    void eventRecived(const QByteArray &message);

    void sendMovedMouseEvent(QPoint point);
    void sendClickMouseEvent(QPoint point);

public slots:
    void start();
    void stop();

signals:
    void isConnectedChanged();

    void remoteMouseMove(QPoint p);
    void waitConnect(QString code);
};

#endif // STREAMER_H
