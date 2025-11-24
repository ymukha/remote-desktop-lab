#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <cstddef>

#include <QObject>

class ScreenCaptureService : public QObject
{
    Q_OBJECT

public:
    explicit ScreenCaptureService(QObject *parent = nullptr);
    ~ScreenCaptureService();

    void start(int intervalMs = 200);
    void stop();

signals:
    void frameCaptured(const std::vector<std::byte>& pixels,
                       int width, int height, int bytesPerLine);

protected:
    void captureLoop(int intervalMs);

    std::atomic<bool> m_IsRunning{ false };
    std::thread m_Thread;
};