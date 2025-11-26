#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QString>

#include <vector>
#include <cstddef>

#include "version.h"
#include "qt_logging_handler.h"

#include "screen_capture_service.h"
#include "network_server.h"
#include "downscale_helper.h"
#include "passthrough_frame_codec.h"
#include "jpeg_frame_codec.h"

const bool useDownscale{ true };
const bool useJpeg{ false };

int main(int argc, char *argv[])
{
    qInstallMessageHandler(QtLogging::fileLogMessageHandler);

    QApplication app(argc, argv);

    QMainWindow mainWindow;

    mainWindow.setWindowTitle(QString("Remote Desktop Server")
        .arg(QString::fromStdString(rdl::core::GetCoreVersion())));

    auto* centralWidget = new QWidget(&mainWindow);
    auto* layout = new QVBoxLayout(centralWidget);


    auto* imageLabel = new QLabel(&mainWindow);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(640, 360); // 16x9

    layout->addWidget(imageLabel, 1);

    centralWidget->setLayout(layout);

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.resize(960, 540); // 16x9
    mainWindow.show();

    PassthroughFrameCodec pthruCodec;
    JPEGFrameCodec jpegCodec;

    NetworkServer server(useJpeg
                            ? static_cast<rdl::core::IFrameCodec&>(jpegCodec)
                            : static_cast<rdl::core::IFrameCodec&>(pthruCodec),
                         &mainWindow);
    ScreenCaptureService capture(&mainWindow);

    QObject::connect(
        &capture,
        &ScreenCaptureService::frameCaptured,
        &server,
        [&server](const std::vector<std::byte>& pixels,
                  int width, int height, int bytesPerLine)
        {
            std::vector<std::byte> outPixels;
            int outWidth { 0 }, outHeight{ 0 },
                outBytesPerLine{ 0 };

            if (useDownscale)
            {
                /* OPTMISATION: downscale to speed-up reception
                 * and scaling/drawing on the client side. Not the
                 * best option as frame looks blurred, but added
                 * useDownscale option to disable when required
                 * (for ex., for testing input logic when it is added) */
                helpers::downscaleFrame(pixels, width, height, bytesPerLine,
                                        outPixels, outWidth, outHeight, outBytesPerLine,
                                        1920, 1080);

                server.sendFrame(outPixels, outWidth, outHeight, outBytesPerLine);
            }
            else
                server.sendFrame(pixels, width, height, bytesPerLine);
        });

#if 0
    QObject::connect(
        &capture,
        &ScreenCaptureService::frameCaptured,
        &server,
        &NetworkServer::sendFrame);

    QObject::connect(
        &capture,
        &ScreenCaptureService::frameCaptured,
        imageLabel,
        [imageLabel](const std::vector<std::byte>& pixels,
                    int width,
                    int height,
                    int bytesPerLine)
        {
            QImage img(reinterpret_cast<const uchar*>(pixels.data()),
                    width, height, bytesPerLine,
                    QImage::Format_ARGB32);

            QImage scaled = img.scaled(imageLabel->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);
            imageLabel->setPixmap(QPixmap::fromImage(scaled));
        }
    );
#endif

    server.start(5000);

    /* It's s lot to send 4k of uncompressed data to
     * client. Especially if client reads frame and
     * immediately scales and draws -> reception delay.
     * Processing queue and sending frame delta may help,
     * but it's absent now. So, meantime just increase
     * capture interval */
    capture.start(200);

    return app.exec();
}