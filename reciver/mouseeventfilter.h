#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include <QObject>
#include <QPointF>
#include <qquickwindow.h>

class EventSerializer
{
public:
    static QJsonObject serialize(QEvent *event)
    {
        QJsonObject json;
        json["type"] = event->type();
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease
            || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            json["button"] = static_cast<qint64>(mouseEvent->button());
            json["buttons"] = static_cast<qint64>(mouseEvent->buttons());
            json["x"] = mouseEvent->pos().x();
            json["y"] = mouseEvent->pos().y();
            json["modifiers"] = static_cast<qint64>(mouseEvent->modifiers());
        } else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            json["key"] = keyEvent->key();
            json["modifiers"] = static_cast<qint64>(keyEvent->modifiers());
            json["text"] = keyEvent->text();
        }
        return json;
    }

    static QEvent *deserialize(const QJsonObject &object, QQuickWindow *window)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
        if (type == 0) {
            return nullptr;
        }

        if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonRelease
            || type == QEvent::MouseButtonRelease || type == QEvent::MouseMove) {
            Qt::MouseButton button = static_cast<Qt::MouseButton>(object["button"].toInteger());
            Qt::MouseButtons buttons = QFlags(
                static_cast<Qt::MouseButton>(object["button"].toInteger()));
            double x = object["x"].toDouble();
            double y = object["y"].toDouble();
            QPointF pos(x, y);
            QPoint globalPos = window->mapToGlobal(QPoint(x, y));
            Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
                object["modifiers"].toInt());

            return new QMouseEvent(type, pos, globalPos, button, buttons, modifiers);
        } else if (type == QEvent::KeyPress || type == QEvent::KeyRelease) {
            int key = object["key"].toInt();
            Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
                object["modifiers"].toInt());
            QString text = object["text"].toString();
            return new QKeyEvent(type, key, modifiers, text);
        }
        return nullptr;
    }

    static QPointF deserializeOnlyMouseMove(const QJsonObject &object)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
        if (type == 0) {
            return QPointF{};
        }

        if (type == QEvent::MouseMove) {
            double x = object["x"].toDouble();
            double y = object["y"].toDouble();
            return QPointF{x, y};
        }
        return QPointF{};
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
