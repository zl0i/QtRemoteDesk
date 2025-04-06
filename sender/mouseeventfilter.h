#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include <QObject>
#include <QPointF>
#include <qquickwindow.h>

class EventFactory
{
public:
    enum EventSource {
        MouseEvent,
        KeyboardEvent,
        WheelEvent,

        FirstVideoReceiverConnected = QEvent::User + 1,
        NewVideoReceiverConnected,
        LastVideoReceiverDisconnected,
        EventReceiverConnected,
        EventReceiverDisconnected,
        UnknowEvent = QEvent::MaxUser,
    };

    static EventSource sourceEvent(const QJsonObject &object)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
        if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonRelease
            || type == QEvent::MouseButtonRelease || type == QEvent::MouseMove) {
            return EventSource::MouseEvent;
        } else if (type == QEvent::KeyPress || type == QEvent::KeyRelease) {
            return EventSource::KeyboardEvent;
        } else if (type > QEvent::User) {
            return static_cast<EventSource>(type);
        }

        return EventSource::UnknowEvent;
    }

    static QJsonObject serialize(QEvent *event)
    {
        QJsonObject json;
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease
            || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            json["type"] = event->type();
            json["button"] = static_cast<qint64>(mouseEvent->button());
            json["buttons"] = static_cast<qint64>(mouseEvent->buttons());
            json["x"] = mouseEvent->pos().x();
            json["y"] = mouseEvent->pos().y();
            json["modifiers"] = static_cast<qint64>(mouseEvent->modifiers());
        } else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            json["type"] = event->type();
            json["key"] = keyEvent->key();
            json["modifiers"] = static_cast<qint64>(keyEvent->modifiers());
            json["text"] = keyEvent->text();
        }
        return json;
    }

    static QMouseEvent deserializeMouseEvent(const QJsonObject &object, QQuickWindow *window)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());

        Qt::MouseButton button = static_cast<Qt::MouseButton>(object["button"].toInteger());
        Qt::MouseButtons buttons = QFlags(
            static_cast<Qt::MouseButton>(object["button"].toInteger()));
        double x = object["x"].toDouble();
        double y = object["y"].toDouble();
        QPointF pos(x, y);
        QPoint globalPos = window->mapToGlobal(QPoint(x, y));
        Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
            object["modifiers"].toInt());

        return QMouseEvent(type, pos, globalPos, button, buttons, modifiers);
    }

    static QKeyEvent deserializeKeyboardEvent(const QJsonObject &object)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
        int key = object["key"].toInt();
        Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
            object["modifiers"].toInt());
        QString text = object["text"].toString();
        return QKeyEvent(type, key, modifiers, text);
    }

    static QKeyEvent deserializeWheelsEvent(const QJsonObject &object)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
        int key = object["key"].toInt();
        Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
            object["modifiers"].toInt());
        QString text = object["text"].toString();
        return QKeyEvent(type, key, modifiers, text);
    }
};

class MouseEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MouseEventFilter(QList<QEvent::Type> filter, QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QList<QEvent::Type> filter;

signals:
    void newEvent(QEvent *event);
};

#endif // MOUSEEVENTFILTER_H
