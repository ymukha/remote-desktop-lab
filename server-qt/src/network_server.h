#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class NetworkServer : public QObject
{
    Q_OBJECT

public:
    explicit NetworkServer(QObject* parent = nullptr);

    bool start(quint16 port);

protected slots:
    void onNewConnection();
    void onClientDisconnect();
    void onReadFromClient();

protected:
    void CleanupClientSocket();

    QTcpServer *m_Server{nullptr};
    QTcpSocket *m_ClientSocket{nullptr};
};