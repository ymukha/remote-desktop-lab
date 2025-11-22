#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

#include "version.h"

int main(int argc, char *argv[])
{
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
    mainWindow.resize(640, 480);
    mainWindow.show();

    return app.exec();
}