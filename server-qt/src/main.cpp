#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <vector>
#include <cstddef>

#include "version.h"
#include "screen_capture.h"
#include "network_server.h"

void fileLogMessageHandler(QtMsgType type,
                           const QMessageLogContext &context,
                           const QString &msg)
{
    Q_UNUSED(context);

    static QFile file("rdl_server_qt.log");
    static bool initialized = false;

    if (!initialized) {
        file.open(QIODevice::Append | QIODevice::Text);
        initialized = true;
    }

    if (!file.isOpen())
        return;

    QTextStream out(&file);

    const char* typeStr = "";
    switch (type) {
    case QtDebugMsg:    typeStr = "DEBUG"; break;
    case QtInfoMsg:     typeStr = "INFO "; break;
    case QtWarningMsg:  typeStr = "WARN "; break;
    case QtCriticalMsg: typeStr = "ERROR"; break;
    case QtFatalMsg:    typeStr = "FATAL"; break;
    }

    out << QDateTime::currentDateTime().toString(Qt::ISODate)
        << " [" << typeStr << "] "
        << msg << "\n";

    out.flush();

    if (type == QtFatalMsg) {
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(fileLogMessageHandler);

    QApplication app(argc, argv);

    QMainWindow mainWindow;

    mainWindow.setWindowTitle("Remote Desktop Server");

    auto* centralWidget = new QWidget(&mainWindow);
    auto* layout = new QVBoxLayout(centralWidget);

    auto* infoLabel = new QLabel(&mainWindow);
    infoLabel->setText(QString("Remote Desktop Lab - Server\nCore version: %1")
                    .arg(QString::fromStdString(rdl::core::GetCoreVersion())));
    infoLabel->setAlignment(Qt::AlignCenter);

    auto* imageLabel = new QLabel(&mainWindow);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(640, 360); // 16x9

    layout->addWidget(infoLabel);
    layout->addWidget(imageLabel, 1);

    centralWidget->setLayout(layout);

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.resize(960, 540); // 16x9
    mainWindow.show();

    auto* timer = new QTimer(&mainWindow);
    QObject::connect(timer, &QTimer::timeout,
        [&](){
            std::vector<std::byte> pixels;
            int width{0}, height{0}, bytesPerLine{0};

            if (rdl::core::CaptureScreen(pixels, width, height, bytesPerLine)
                && width > 0 && height > 0)
                {
                    QImage img(reinterpret_cast<const uchar*>(pixels.data()), 
                                width, height, static_cast<qsizetype>(bytesPerLine),
                                QImage::Format_ARGB32);

                    QImage scaled = img.scaled(imageLabel->size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);
                    imageLabel->setPixmap(QPixmap::fromImage(scaled));
                }
                else
                {
                    imageLabel->setText("Screen capture failed");
                }
        });

    NetworkServer server(&mainWindow);
    server.start(5000);

    timer->setTimerType(Qt::PreciseTimer);
    timer->start(16.6); // 60fps, on the boundary of Win non-hires timer (15.6 ms)
    return app.exec();
}