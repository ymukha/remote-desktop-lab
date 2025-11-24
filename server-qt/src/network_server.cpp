#include "network_server.h"

#include <QByteArray>
#include <QDebug>

#include <cstddef>

#include "frame_protocol.h"

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
    cleanupClientSocket(); // Drop existing connection

    // Accept pending connection
    m_ClientSocket = m_Server->nextPendingConnection();

    qInfo() << "NetworkServer: client connected: "
            << m_ClientSocket->peerAddress().toString()
            << ":" << m_ClientSocket->peerPort();

    connect(m_ClientSocket, &QTcpSocket::disconnected,
            this, &NetworkServer::onClientDisconnect);
    connect(m_ClientSocket, &QTcpSocket::readyRead,
            this, &NetworkServer::onReadFromClient);

    // Send test frame
    sendTestFrame();
}

void NetworkServer::cleanupClientSocket()
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

void NetworkServer::sendTestFrame()
{
    if (!m_ClientSocket)
        return;

    const uint32_t width{ 800 },
                   height { 600 },
                   bytesPerPixel { 4 },
                   dataSize{ width * height * bytesPerPixel };

    auto header = rdl::core::makeFrameHeader(width, height,
                                             rdl::core::FramePixelFormat::BGRA32,
                                             dataSize);

    QByteArray payload;
    payload.resize(dataSize);
    
    auto* data = reinterpret_cast<std::byte*>(payload.data());
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)        
        {
            auto idx{ (static_cast<size_t>(y) * width + x) * 4 }; // 4 bites step

            data[idx    ] = static_cast<std::byte>(x % 256);        // B
            data[idx + 1] = static_cast<std::byte>(y % 256);        // G
            data[idx + 2] = static_cast<std::byte>((y + x) % 256);  // R
            data[idx + 3] = static_cast<std::byte>(255);            // A
        }
    }

    qInfo() << "NetworkServer: sending test frame:"
            << " w=" << width
            << ", h=" << height
            << ", dataSize=" << dataSize;

    m_ClientSocket->write(reinterpret_cast<const char*>(&header), sizeof(header));
    m_ClientSocket->write(payload);
    m_ClientSocket->flush();
}

void NetworkServer::sendFrame(const std::vector<std::byte>& pixels,
                              int width, int height, int bytesPerLine)
{
    if (!m_ClientSocket)
        return;

    qInfo() << "NetworkServer: sending frame:"
            << " w=" << width
            << ", h=" << height;

    auto header = rdl::core::makeFrameHeader(width, height,
                                             rdl::core::FramePixelFormat::BGRA32,
                                             bytesPerLine * height);

    m_ClientSocket->write(reinterpret_cast<const char*>(&header), sizeof(header));
    m_ClientSocket->write(reinterpret_cast<const char*>(pixels.data()), header.dataSize);

    qInfo() << "end sending";
}