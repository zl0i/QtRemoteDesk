#ifndef FPSRATER_H
#define FPSRATER_H

#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QWebSocket>

class FpsRater : public QObject
{
    Q_OBJECT
public:
    explicit FpsRater(QWebSocket *socket, QObject *parent = nullptr);

private:
    QTimer timer;

    bool busy = false;
    int frameDropCount = 0;

    int frameInterval = 100; // 10 FPS
    int maxInterval = 1000;  // min 1 FPS
    int minInterval = 33;    // max ~30 FPS

    int jpegQuality = 90;      // Начальное качество JPEG
    const int minQuality = 30; // Максимальное сжатие
    const int maxQuality = 90; // Лучшее качество

private slots:
    void onBytesWritten();
    void trySendFrame();

public slots:
    void start();
    void stop();

signals:
    void sendFrame(int quality);
};

#endif // FPSRATER_H
