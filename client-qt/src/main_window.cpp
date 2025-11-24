#include "main_window.h"

#include <QVBoxLayout>

#include "version.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QString("Remote Desktop Client v%1")
        .arg(QString::fromStdString(rdl::core::GetCoreVersion())));

    auto central = new QWidget(this);
    auto layout = new QVBoxLayout(central);

    m_FrameView = new FrameView(central);
    layout->addWidget(m_FrameView);

    setCentralWidget(central);

    m_Client = new NetworkClient(this);

    connect(m_Client, &NetworkClient::frameReceived,
            m_FrameView, &FrameView::setFrame);

    m_Client->connectToServer("127.0.0.1", 5000);
}