#include "screen_capture_service.h"

#include<QMetaObject>

#include <chrono>

#include "screen_capture.h"

ScreenCaptureService::ScreenCaptureService(QObject *parent)
    : QObject(parent)
{
}

ScreenCaptureService::~ScreenCaptureService()
{
    stop();
}

void ScreenCaptureService::start(int intervalMs)
{
    if (m_IsRunning.exchange(true))
        return;

    m_Thread = std::thread([this, intervalMs]()
    {
        captureLoop(intervalMs);
    });
}

void ScreenCaptureService::stop()
{
    if (!m_IsRunning.exchange(false))
        return;

    if (m_Thread.joinable())
        m_Thread.join();
}

void ScreenCaptureService::captureLoop(int intervalMs)
{
    using namespace std::chrono;

    while (m_IsRunning.load())
    {
        std::vector<std::byte> pixels;
        int width{ 0 }, height { 0 },
            bytesPerLine{ 0 };

        if (rdl::core::CaptureScreen(pixels, width, height, bytesPerLine)
                    && width > 0 && height > 0)
        {
            QMetaObject::invokeMethod(this,
                [this, pixels = std::move(pixels), width, height, bytesPerLine]
                {
                    emit frameCaptured(pixels, width, height, bytesPerLine);
                },
                Qt::QueuedConnection
            );
        }

        std::this_thread::sleep_for(milliseconds(intervalMs));
    }
}