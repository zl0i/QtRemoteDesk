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

#include "fpsrater.h"
#include "eventmanager.h"

class AppStreamer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ readIsConnected NOTIFY isConnectedChanged FINAL)
public:
    AppStreamer(QObject *parent = nullptr);

    void setQmlEngine(QQmlApplicationEngine *engine);

    EventManager *eventFilter();

    bool readIsConnected() { return isConnected; }

private:
    QQuickWindow *window;

    QWebSocket imageSocket;
    QWebSocket eventSocket;
    bool isConnected{false};

    EventManager eventManager;
    QNetworkAccessManager manager;
    FpsRater fpsRater;

private slots:
    void onConnectedVideo();
    void onDisconnectedVideo();
    void onConnectedEvent();
    void onDisconnectedEvent();
    void onError(QAbstractSocket::SocketError error);

    void captureAndSendScreen(int quality);
    void eventRecived(const QByteArray &message);
    void sendEvent(QEvent *event);
    void serverEvent(EventFactory::EventSource e);

public slots:
    void start();
    void stop();

signals:
    void isConnectedChanged();

    void remoteMouseMove(QPoint p);
    void waitConnect(QString code);

    void videoReceiverConnected();
    void eventReceiverConnected();
};

#endif // STREAMER_H
