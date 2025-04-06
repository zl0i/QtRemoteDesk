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

class VideoReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray videoFrame READ videoFrame NOTIFY frameUpdated)
    Q_PROPERTY(bool isConnected MEMBER m_isConnected NOTIFY connectionChanged)

public:
    explicit VideoReceiver(QObject *parent = nullptr);

    void setQmlEngine(QQmlApplicationEngine *engine);
    QByteArray videoFrame() const;
    MouseEventFilter *eventFilter() { return &mouseEvent; };

private:
    QQuickWindow *window;
    QWebSocket imageSocket;
    QUrl eventUrl;
    QWebSocket eventSocket;
    QByteArray m_currentFrame;
    bool m_isConnected = false;
    MouseEventFilter mouseEvent;
    QNetworkAccessManager manager;

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
};

#endif // STREAMER_H
