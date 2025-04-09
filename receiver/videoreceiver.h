#ifndef VIDEORECEIVER_H
#define VIDEORECEIVER_H

#include <QImage>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QWebSocket>

#include "eventmanager.h"
#include "remoteimageprovider.h"

class VideoReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected MEMBER m_isConnected NOTIFY connectionChanged)
    Q_PROPERTY(QString code READ readCode NOTIFY codeChanged)

public:
    explicit VideoReceiver(QObject *parent = nullptr);
    virtual ~VideoReceiver();

    void setQmlEngine(QQmlApplicationEngine *engine);
    EventManager *eventFilter() { return &eventManager; };
    QQuickImageProvider *imageProvider() { return imageProivder; };
    QString readCode() { return code; }

private:
    QQuickWindow *window;
    QWebSocket imageSocket;
    QUrl eventUrl;
    QWebSocket eventSocket;
    bool m_isConnected = false;
    EventManager eventManager;
    QNetworkAccessManager manager;

    QString code;
    RemoteImageProvider *imageProivder;

private slots:
    void onEventReceived(const QByteArray &message);

    void sendEvent(QJsonObject object);

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
    void connectionChanged();
    void codeChanged();
};

#endif // VIDEORECEIVER_H
