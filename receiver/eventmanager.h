#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QObject>
#include <QTimer>

#include "eventserializer.h"

class EventManager : public QObject
{
    Q_OBJECT
public:
    explicit EventManager(QList<QEvent::Type> filter, QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;
    void setWindow(const QQuickWindow *window) { this->window = window; };

protected:
    EventSerializer serializer;

private:
    const QQuickWindow *window = nullptr;
    QList<QEvent::Type> filter;

    const bool isFilterMousePress = false;
    QTimer timerMouse;
    bool isHaveMousePress = false;
    QJsonObject sMouseEvent;

    const bool isFilterKeyPress = false;
    QTimer timerKey;
    bool isHaveKeyPress = false;
    QJsonObject sKeyEvent;

    const bool isFilterMouseMove = false;
    QTimer timerMove;

signals:
    void newEvent(QJsonObject object);
};

#endif // EVENTMANAGER_H
