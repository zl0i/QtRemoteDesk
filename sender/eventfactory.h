#ifndef EVENTFACTORY_H
#define EVENTFACTORY_H

#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include <QObject>
#include <QQuickWindow>

class EventFactory
{
private:
    static const inline int MAX_WIDTH_SAMPLES = 65535;
    static const inline int MAX_HEIGHT_SAMPLES = 65535;

public:
    enum EventSource {
        MouseEvent,
        KeyboardEvent,
        WheelEvent,

        MouseClick = QEvent::User + 1,
        KeyClick,

        FirstVideoReceiverConnected = 1501,
        NewVideoReceiverConnected,
        LastVideoReceiverDisconnected,
        EventReceiverConnected,
        EventReceiverDisconnected,
        UnknowEvent = QEvent::MaxUser,
    };

    EventFactory();

    static EventSource sourceEvent(const QJsonObject &object);
    static QJsonObject serialize(QEvent *event, QSize windowSize);
    static QEvent *deserializeMouseEvent(const QJsonObject &object, QQuickWindow *window);
    static QEvent *deserializeKeyboardEvent(const QJsonObject &object);
    static QPair<QMouseEvent *, QMouseEvent *> deserializeMouseClick(const QJsonObject &object,
                                                                     QQuickWindow *window);
    static QPair<QKeyEvent *, QKeyEvent *> deserializeKeyboardClick(const QJsonObject &object);
    static QWheelEvent *deserializeWheelEvent(const QJsonObject &object, QQuickWindow *window);
};

#endif // EVENTFACTORY_H
