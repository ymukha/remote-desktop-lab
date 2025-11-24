#include <QApplication>

#include <QDebug>

#include "qt_logging_handler.h"
#include "main_window.h"


int main(int argc, char *argv[])
{
    qInstallMessageHandler(QtLogging::fileLogMessageHandler);

    QApplication app(argc, argv);

    MainWindow mainWindow;

    mainWindow.resize(1024, 768);
    mainWindow.show();

    return app.exec();
}