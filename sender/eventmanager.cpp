#include "eventmanager.h"

EventManager::EventManager(QList<QEvent::Type> filter, QObject *parent)
    : QObject{parent}
    , filter(filter)
{}

bool EventManager::eventFilter(QObject *obj, QEvent *event)
{
    if (filter.contains(event->type())) {
        emit newEvent(event);
    }
    return QObject::eventFilter(obj, event);
}

void EventManager::setQQuickWindow(QQuickWindow *w)
{
    this->window = w;
}

void EventManager::handleRemoteEvent(const QJsonObject &object)
{
    if (!window)
        return;

    auto source = EventFactory::sourceEvent(object);
    if (source == EventFactory::MouseEvent) {
        auto *event = EventFactory::deserializeMouseEvent(object, window);
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *move = static_cast<QMouseEvent *>(event);
            emit remoteMouseMove(move->pos());
        }
        QCoreApplication::postEvent(window, event);
    } else if (source == EventFactory::KeyboardEvent) {
        auto *event = EventFactory::deserializeKeyboardEvent(object);
        QCoreApplication::postEvent(window, event);
    } else if (source == EventFactory::MouseClick) {
        auto events = EventFactory::deserializeMouseClick(object, window);
        QCoreApplication::postEvent(window, events.first);
        QCoreApplication::postEvent(window, events.second);
    } else if (source == EventFactory::KeyClick) {
        auto events = EventFactory::deserializeKeyboardClick(object);
        QCoreApplication::postEvent(window, events.first);
        QCoreApplication::postEvent(window, events.second);
    } else if (source == EventFactory::WheelEvent) {
        QWheelEvent *event = EventFactory::deserializeWheelEvent(object, window);
        QCoreApplication::postEvent(window, event);
    } else if (source == EventFactory::FirstVideoReceiverConnected) {
        emit serverEvent(EventFactory::FirstVideoReceiverConnected);
    } else if (source == EventFactory::NewVideoReceiverConnected) {
        emit serverEvent(EventFactory::NewVideoReceiverConnected);
    } else if (source == EventFactory::LastVideoReceiverDisconnected) {
        emit serverEvent(EventFactory::LastVideoReceiverDisconnected);
    } else if (source == EventFactory::EventReceiverConnected) {
        emit serverEvent(EventFactory::EventReceiverConnected);
    } else if (source == EventFactory::EventReceiverDisconnected) {
        emit serverEvent(EventFactory::EventReceiverDisconnected);
    }
}
