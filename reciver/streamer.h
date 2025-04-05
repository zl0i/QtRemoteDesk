#ifndef STREAMER_H
#define STREAMER_H

#include <QImage>
#include <QObject>
#include <QWebSocket>
#include "mouseeventfilter.h"

class VideoReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray videoFrame READ videoFrame NOTIFY frameUpdated)
    Q_PROPERTY(bool isConnected MEMBER m_isConnected NOTIFY connectionChanged)

public:
    explicit VideoReceiver(QObject *parent = nullptr);

    QByteArray videoFrame() const;
    MouseEventFilter *eventFilter() { return &mouseEvent; };

private:
    QWebSocket imageSocket;
    QWebSocket eventSocket;
    QByteArray m_currentFrame;
    bool m_isConnected = false;
    MouseEventFilter mouseEvent;

private slots:
    void onBinaryMessageReceived(const QByteArray &message);
    void onTextMessageReceived(const QString &message);
    void onEventMessageReceived(const QByteArray &message);

    void onConnectedVideo();
    void onDisconnectedVideo();

    void onConnectedEvent();
    void onDisconnectedEvent();
    void sendMovedMouseEvent(QPoint point);
    void sendClickMouseEvent(QPoint point);

public slots:
    void connectToServer();
    void disconnectFromServer();

signals:
    void frameUpdated();
    void mouseClick(QPoint p);
    void mouseMove(QPoint p);
    void connectionChanged();
};

#endif // STREAMER_H
