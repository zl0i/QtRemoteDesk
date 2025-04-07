#include "fpsrater.h"

FpsRater::FpsRater(QWebSocket *socket, QObject *parent)
    : QObject{parent}
{
    timer.setInterval(frameInterval); // 10 FPS
    connect(socket, &QWebSocket::bytesWritten, this, &FpsRater::onBytesWritten);
    connect(&timer, &QTimer::timeout, this, &FpsRater::trySendFrame);
}

void FpsRater::onBytesWritten()
{
    busy = false;

    // Если успешно отправили — ускоряем (но не резко)
    if (frameInterval > minInterval) {
        frameInterval -= 10;
        frameInterval = qMax(frameInterval, minInterval);
        timer.setInterval(frameInterval);
        qDebug() << "Speeding up to" << 1000.0 / frameInterval << "FPS";
    }

    if (jpegQuality < maxQuality) {
        jpegQuality += 5;
        jpegQuality = qMin(jpegQuality, maxQuality);
        qDebug() << "Improving JPEG quality to" << jpegQuality;
    }
}

void FpsRater::trySendFrame()
{
    if (busy) {
        frameDropCount++;

        if (frameDropCount >= 3) {
            // Уменьшаем FPS
            if (frameInterval < maxInterval) {
                frameInterval += 50;
                timer.setInterval(frameInterval);
                qDebug() << "Slowing down to" << 1000.0 / frameInterval << "FPS";
            }

            // Уменьшаем качество
            if (jpegQuality > minQuality) {
                jpegQuality -= 10;
                jpegQuality = qMax(jpegQuality, minQuality);
                qDebug() << "Reducing JPEG quality to" << jpegQuality;
            }
            frameDropCount = 0;
        }
        return;
    }
    emit sendFrame(jpegQuality);
}

void FpsRater::start()
{
    busy = false;
    frameInterval = minInterval;
    jpegQuality = maxQuality;
    timer.start(frameInterval);
}

void FpsRater::stop()
{
    busy = false;
    timer.stop();
}
