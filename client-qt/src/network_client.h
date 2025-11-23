#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);

private slots:
    void onServerConnect();
    void onServerDisconnect();
    void onReadFromServer();
    void onSocketError(QAbstractSocket::SocketError);

protected:
    QTcpSocket *m_Socket{nullptr};
};