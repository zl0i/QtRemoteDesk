#ifndef EVENTSERIALIZER_H
#define EVENTSERIALIZER_H

#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include <QObject>
#include <QPoint>
#include <QQuickWindow>
#include <QSize>

class EventSerializer
{
private:
    static const inline int MAX_WIDTH_SAMPLES = 65535;
    static const inline int MAX_HEIGHT_SAMPLES = 65535;
    QPoint lastMousePoint;

public:
    enum RemoteUserEvent {
        MouseClick = QEvent::User + 1,
        KeyClick,
    };

    EventSerializer();

    QJsonObject serialize(QEvent *event, QSize windowSize);
    QEvent *deserialize(const QJsonObject &object, QQuickWindow *window);
    QPointF deserializeMouseMove(const QJsonObject &object, QSize windowSize);
};

#endif // EVENTSERIALIZER_H
