#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <vector>
#include <cstddef>

class NetworkServer : public QObject
{
    Q_OBJECT

public:
    explicit NetworkServer(QObject* parent = nullptr);

    bool start(quint16 port);

public slots:
    void sendFrame(const std::vector<std::byte>& pixels,
                   int width, int height, int bytesPerLine);

protected slots:
    void onNewConnection();
    void onClientDisconnect();
    void onReadFromClient();

protected:
    void cleanupClientSocket();
    void sendTestFrame();

    QTcpServer *m_Server{nullptr};
    QTcpSocket *m_ClientSocket{nullptr};
};