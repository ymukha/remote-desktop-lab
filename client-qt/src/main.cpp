#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "version.h"
#include "qt_logging_handler.h"

#include "network_client.h"


int main(int argc, char *argv[])
{
    qInstallMessageHandler(QtLogging::fileLogMessageHandler);

    QApplication app(argc, argv);

    QMainWindow mainWindow;

    mainWindow.setWindowTitle("Remote Desktop Client");

    auto* centralWidget = new QWidget(&mainWindow);
    auto* layout = new QVBoxLayout(centralWidget);

    auto* label = new QLabel(&mainWindow);
    label->setText(QString("Remote Desktop Lab - Client\nCore version: %1")
                    .arg(QString::fromStdString(rdl::core::GetCoreVersion())));
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);

    centralWidget->setLayout(layout);

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.resize(640, 360); // 16x9
    mainWindow.show();

    NetworkClient client(&mainWindow);
    client.connectToServer("127.0.0.1", 5000);

    return app.exec();
}