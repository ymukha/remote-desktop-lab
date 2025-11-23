#include "network_server.h"

#include <QByteArray>
#include <QDebug>

NetworkServer::NetworkServer(QObject* parent)
    : QObject(parent)
    , m_Server{ new QTcpServer(this) }
{
    connect(m_Server, &QTcpServer::newConnection,
            this, &NetworkServer::onNewConnection);
}

bool NetworkServer::start(quint16 port)
{
    if (!m_Server->listen(QHostAddress::Any, port))
    {
        qWarning() << "NetworkServer: listen failed on port=" << port
                   << ", error=" << m_Server->errorString();
        return false;
    }

    qInfo() << "NetworkServer: listening on port=" << port;

    return true;
}

void NetworkServer::onNewConnection()
{
    // Support only 1 connection at a time
    CleanupClientSocket(); // Drop existing connection

    // Accept pending connection
    m_ClientSocket = m_Server->nextPendingConnection();

    qInfo() << "NetworkServer: client connected: "
            << m_ClientSocket->peerAddress().toString()
            << ":" << m_ClientSocket->peerPort();

    connect(m_ClientSocket, &QTcpSocket::disconnected,
            this, &NetworkServer::onClientDisconnect);
    connect(m_ClientSocket, &QTcpSocket::readyRead,
            this, &NetworkServer::onReadFromClient);

    // Send greetings
    m_ClientSocket->write("Welcome to server!");
    m_ClientSocket->flush();
}

void NetworkServer::CleanupClientSocket()
{
    if (!m_ClientSocket)
        return;

    m_ClientSocket->disconnect(this);
    m_ClientSocket->close();
    m_ClientSocket->deleteLater();
    m_ClientSocket = nullptr;
}

void NetworkServer::onClientDisconnect()
{
    qInfo() << "NetworkServer: client disconnected";

    if (!m_ClientSocket)
        return;

    m_ClientSocket->deleteLater();
    m_ClientSocket = nullptr;
}

void NetworkServer::onReadFromClient()
{
    if (!m_ClientSocket)
        return;

    QByteArray data = m_ClientSocket->readAll();

    qInfo() << "NetworkServer: received from client:" << data;
}