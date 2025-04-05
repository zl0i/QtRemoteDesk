#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <QEvent>
#include <QMouseEvent>
#include <QObject>

class MouseEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MouseEventFilter(QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void mouseClicked(QPoint point);
    void mouseMoved(QPoint point);
};

#endif // MOUSEEVENTFILTER_H
