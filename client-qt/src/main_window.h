#include <QMainWindow>
#include <QWidget>

#include "network_client.h"
#include "frame_view.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    NetworkClient *m_Client{ nullptr };
    FrameView *m_FrameView{ nullptr };
};