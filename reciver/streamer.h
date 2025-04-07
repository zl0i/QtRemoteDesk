#ifndef STREAMER_H
#define STREAMER_H

#include <QImage>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QWebSocket>

#include "mouseeventfilter.h"
#include "remoteimageprovider.h"

class VideoReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected MEMBER m_isConnected NOTIFY connectionChanged)
    Q_PROPERTY(QString code READ readCode NOTIFY codeChanged)

public:
    explicit VideoReceiver(QObject *parent = nullptr);

    void setQmlEngine(QQmlApplicationEngine *engine);
    MouseEventFilter *eventFilter() { return &mouseEvent; };
    QQuickImageProvider *imageProvider() { return imageProivder; };
    QString readCode() { return code; }

private:
    QQuickWindow *window;
    QWebSocket imageSocket;
    QUrl eventUrl;
    QWebSocket eventSocket;
    bool m_isConnected = false;
    MouseEventFilter mouseEvent;
    QNetworkAccessManager manager;
    EventSerializer eventSerializer;

    QString code;
    RemoteImageProvider *imageProivder = new RemoteImageProvider;

private slots:
    void onVideoReceived(const QByteArray &message);
    void onEventReceived(const QByteArray &message);

    void sendEvent(QEvent *event);

    void onConnectedVideo();
    void onDisconnectedVideo();

    void onConnectedEvent();
    void onDisconnectedEvent();

public slots:
    void connectVideo(QString code);
    void connectEvent();
    void disconnectFromServer();

signals:
    void frameUpdated();
    void mouseMove(QPointF p);
    void connectionChanged();
    void codeChanged();
};

#endif // STREAMER_H
