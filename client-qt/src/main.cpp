#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "version.h"
#include "network_client.h"

void fileLogMessageHandler(QtMsgType type,
                           const QMessageLogContext &context,
                           const QString &msg)
{
    Q_UNUSED(context);

    static QFile file("rdl_client_qt.log");
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