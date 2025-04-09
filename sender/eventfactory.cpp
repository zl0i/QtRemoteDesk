#include "eventfactory.h"

EventFactory::EventFactory() {}

QJsonObject EventFactory::serialize(QEvent *event, QSize windowSize)
{
    QJsonObject json;
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease
        || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        json["type"] = event->type();
        json["button"] = static_cast<qint64>(mouseEvent->button());
        json["buttons"] = static_cast<qint64>(mouseEvent->buttons());
        json["x"] = mouseEvent->pos().x() * MAX_WIDTH_SAMPLES / windowSize.width();
        json["y"] = mouseEvent->pos().y() * MAX_HEIGHT_SAMPLES / windowSize.height();
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

QEvent *EventFactory::deserializeMouseEvent(const QJsonObject &object, QQuickWindow *window)
{
    QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());

    Qt::MouseButton button = static_cast<Qt::MouseButton>(object["button"].toInteger());
    Qt::MouseButtons buttons = QFlags(static_cast<Qt::MouseButton>(object["buttons"].toInteger()));
    double x = object["x"].toDouble() * window->size().width() / MAX_WIDTH_SAMPLES;
    double y = object["y"].toDouble() * window->size().height() / MAX_HEIGHT_SAMPLES;
    QPointF pos(x, y);
    QPoint globalPos = window->mapToGlobal(QPoint(x, y));
    Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
        object["modifiers"].toInt());
    return new QMouseEvent(type, pos, globalPos, button, buttons, modifiers);
}

QEvent *EventFactory::deserializeKeyboardEvent(const QJsonObject &object)
{
    QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
    int key = object["key"].toInt();
    Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
        object["modifiers"].toInt());
    QString text = object["text"].toString();
    return new QKeyEvent(type, key, modifiers, text);
}

QPair<QMouseEvent *, QMouseEvent *> EventFactory::deserializeMouseClick(const QJsonObject &object,
                                                                        QQuickWindow *window)
{
    Qt::MouseButton button = static_cast<Qt::MouseButton>(object["button"].toInteger());
    Qt::MouseButtons buttons = QFlags(static_cast<Qt::MouseButton>(object["buttons"].toInteger()));
    double x = object["x"].toDouble() * window->size().width() / MAX_WIDTH_SAMPLES;
    double y = object["y"].toDouble() * window->size().height() / MAX_HEIGHT_SAMPLES;
    QPointF pos(x, y);
    QPoint globalPos = window->mapToGlobal(QPoint(x, y));
    Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
        object["modifiers"].toInt());

    QMouseEvent *press
        = new QMouseEvent(QEvent::MouseButtonPress, pos, globalPos, button, buttons, modifiers);
    QMouseEvent *release
        = new QMouseEvent(QEvent::MouseButtonRelease, pos, globalPos, button, buttons, modifiers);

    return {press, release};
}

QPair<QKeyEvent *, QKeyEvent *> EventFactory::deserializeKeyboardClick(const QJsonObject &object)
{
    int key = object["key"].toInt();
    Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(
        object["modifiers"].toInt());
    QString text = object["text"].toString();

    QKeyEvent *press = new QKeyEvent(QEvent::KeyPress, key, modifiers, text);
    QKeyEvent *release = new QKeyEvent(QEvent::KeyRelease, key, modifiers, text);

    return {press, release};
}

QWheelEvent *EventFactory::deserializeWheelEvent(const QJsonObject &object, QQuickWindow *window)
{
    int x = object.value("x").toInt() * window->size().width() / MAX_WIDTH_SAMPLES;
    int y = object.value("y").toInt() * window->size().height() / MAX_HEIGHT_SAMPLES;
    QPointF pos(x, y);
    QPoint globalPos = window->mapToGlobal(QPoint(x, y));

    int pixelDeltaX = object.value("delta_x").toInt();
    int pixelDeltaY = object.value("delta_y").toInt();

    int angleDeltaX = object.value("angle_x").toInt();
    int angleDeltaY = object.value("angle_y").toInt();

    Qt::ScrollPhase phase = static_cast<Qt::ScrollPhase>(object.value("phase").toInt());
    int inverted = object.value("inverted").toBool();

    return new QWheelEvent(pos,
                           globalPos,
                           QPoint{pixelDeltaX, pixelDeltaY},
                           QPoint{angleDeltaX, angleDeltaY},
                           Qt::NoButton,
                           Qt::NoModifier,
                           phase,
                           inverted);
}

EventFactory::EventSource EventFactory::sourceEvent(const QJsonObject &object)
{
    QEvent::Type type = static_cast<QEvent::Type>(object.value("type").toInteger());
    if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonRelease
        || type == QEvent::MouseButtonRelease || type == QEvent::MouseMove) {
        return EventSource::MouseEvent;
    } else if (type == QEvent::KeyPress || type == QEvent::KeyRelease) {
        return EventSource::KeyboardEvent;
    } else if (type == QEvent::Wheel) {
        return EventSource::WheelEvent;
    } else if (type > QEvent::User) {
        return static_cast<EventSource>(type);
    }

    return EventSource::UnknowEvent;
}
