#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include <QObject>
#include <QPair>
#include <QPointF>
#include <QWheelEvent>

#include "eventfactory.h"

class EventManager : public QObject
{
    Q_OBJECT
public:
    explicit EventManager(QList<QEvent::Type> filter, QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;
    void setQQuickWindow(QQuickWindow *w);

    void handleRemoteEvent(const QJsonObject &object);

private:
    QList<QEvent::Type> filter;
    EventFactory eventFactory;

    QQuickWindow *window{nullptr};

signals:
    void newEvent(QEvent *event);

    void remoteMouseMove(QPoint p);
    void serverEvent(EventFactory::EventSource e);
};

#endif // EVENTMANAGER_H
