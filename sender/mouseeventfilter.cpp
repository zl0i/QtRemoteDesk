#include "mouseeventfilter.h"

MouseEventFilter::MouseEventFilter(QList<QEvent::Type> filter, QObject *parent)
    : QObject{parent}
    , filter(filter)
{}

bool MouseEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (filter.contains(event->type())) {
        emit newEvent(event);
    }
    return QObject::eventFilter(obj, event);
}
