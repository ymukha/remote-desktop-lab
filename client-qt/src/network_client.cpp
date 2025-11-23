#include <network_client.h>

#include <QDebug>

NetworkClient::NetworkClient(QObject* parent)
    : QObject(parent)
    , m_Socket{ new QTcpSocket(this) }
{
    connect(m_Socket, &QTcpSocket::connected,
            this, &NetworkClient::onServerConnect);
    connect(m_Socket, &QTcpSocket::disconnected,
            this, &NetworkClient::onServerDisconnect);
    connect(m_Socket, &QTcpSocket::readyRead,
            this, &NetworkClient::onReadFromServer);
    connect(m_Socket, &QTcpSocket::errorOccurred,
            this, &NetworkClient::onSocketError);
}

void NetworkClient::connectToServer(const QString& host, quint16 port)
{
    qInfo() << "NetworkClient: connecting to" << host << ":" << port;

    m_Socket->connectToHost(host, port);
}

void NetworkClient::onServerConnect()
{
    qInfo() << "NetworkClient: connected";

    m_Socket->write("Thx!");
    m_Socket->flush();
}

void NetworkClient::onServerDisconnect()
{
    qInfo() << "NetworkClient: disconnected";
}

void NetworkClient::onReadFromServer()
{
    QByteArray data = m_Socket->readAll();

     qInfo() << "NetworkClient: received from server: " << data;
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError)
{
    qWarning() << "NetworkClient: error=" << m_Socket->errorString();
}