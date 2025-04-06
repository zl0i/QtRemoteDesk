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
private:
    static const inline int MAX_WIDTH_SAMPLES = 65535;
    static const inline int MAX_HEIGHT_SAMPLES = 65535;

public:
    static QJsonObject serialize(QEvent *event, QSize windowSize)
    {
        QJsonObject json;
        json["type"] = event->type();
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease
            || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            json["button"] = static_cast<qint64>(mouseEvent->button());
            json["buttons"] = static_cast<qint64>(mouseEvent->buttons());
            json["x"] = mouseEvent->pos().x() * MAX_WIDTH_SAMPLES / windowSize.width();
            json["y"] = mouseEvent->pos().y() * MAX_HEIGHT_SAMPLES / windowSize.height();
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
            double x = object["x"].toDouble() * window->size().width() / MAX_WIDTH_SAMPLES;
            double y = object["y"].toDouble() * window->size().height() / MAX_HEIGHT_SAMPLES;
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

    static QPointF deserializeOnlyMouseMove(const QJsonObject &object, QSize windowSize)
    {
        QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
        if (type == 0) {
            return QPointF{};
        }

        if (type == QEvent::MouseMove) {
            double x = object["x"].toDouble() * windowSize.width() / MAX_WIDTH_SAMPLES;
            double y = object["y"].toDouble() * windowSize.height() / MAX_HEIGHT_SAMPLES;
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
