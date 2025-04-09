#include "mouseeventfilter.h"

MouseEventFilter::MouseEventFilter(QList<QEvent::Type> filter, QObject *parent)
    : QObject{parent}
    , filter(filter)
    , isFilterMousePress(filter.contains(QEvent::MouseButtonPress))
    , isFilterKeyPress(filter.contains(QEvent::KeyPress))
{
    timerMouse.setInterval(150);
    timerMouse.setSingleShot(true);
    connect(&timerMouse, &QTimer::timeout, [this]() {
        if (!sMouseEvent.isEmpty()) {
            isHaveMousePress = false;
            emit newEvent(sMouseEvent);
        }
    });

    timerKey.setInterval(150);
    timerKey.setSingleShot(true);
    connect(&timerKey, &QTimer::timeout, [this]() {
        if (!sKeyEvent.isEmpty()) {
            isHaveKeyPress = false;
            emit newEvent(sKeyEvent);
        }
    });
}

bool MouseEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    QSize size = window ? window->size() : QSize{};
    if (filter.contains(event->type())
        && !(event->type() == QEvent::MouseButtonPress && isFilterMousePress)
        && !(event->type() == QEvent::KeyPress && isFilterKeyPress)) {
        emit newEvent(serializer.serialize(event, size));
    }
    if (isHaveMousePress && event->type() == QEvent::MouseButtonRelease) {
        timerMouse.stop();
        sMouseEvent["type"] = EventSerializer::MouseClick;
        isHaveMousePress = false;
        emit newEvent(sMouseEvent);
    } else if (isFilterMousePress && event->type() == QEvent::MouseButtonPress
               && !timerMouse.isActive()) {
        isHaveMousePress = true;
        timerMouse.start();
        sMouseEvent = serializer.serialize(event, size);
    } else if (isHaveKeyPress && event->type() == QEvent::KeyRelease) {
        timerKey.stop();
        sKeyEvent["type"] = EventSerializer::KeyClick;
        isHaveKeyPress = false;
        emit newEvent(sKeyEvent);
    } else if (isFilterKeyPress && event->type() == QEvent::KeyPress && !timerKey.isActive()) {
        isHaveKeyPress = true;
        timerKey.start();
        sKeyEvent = serializer.serialize(event, size);
    }
    return QObject::eventFilter(obj, event);
}
