#include "mouseeventfilter.h"

MouseEventFilter::MouseEventFilter(QObject *parent)
    : QObject{parent}
{}

bool MouseEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        emit mouseClicked(mouseEvent->pos());
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        emit mouseMoved(mouseEvent->pos());
    }
    return QObject::eventFilter(obj, event);
}
