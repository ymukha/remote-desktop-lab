#include "network_client.h"

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

    m_Worker = std::thread([this]() { workerLoop(); });
}

NetworkClient::~NetworkClient()
{
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_StopWorker = true;
    }

    m_QueueCv.notify_all();

    if (m_Worker.joinable())
        m_Worker.join();
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
    while(m_Socket->bytesAvailable() > 0)
    {
        const auto frameHeaderSize{ sizeof(rdl::core::FrameHeader) };

        if (m_State == ReadState::Header)
        {
            const auto bytesToRead = static_cast<quint64>(frameHeaderSize) - m_HeaderBuffer.size();
            auto chunk = m_Socket->read(bytesToRead);
            m_HeaderBuffer.append(chunk);

            if (m_HeaderBuffer.size() < static_cast<int>(frameHeaderSize))
                return; // Header has not been completely received yet
            
            std::memcpy(&m_Header, m_HeaderBuffer.constData(), frameHeaderSize);

            if (m_Header.magic != rdl::core::FRAME_MAGIC)
            {
                qWarning() << "NetworkClient: invalid frame magic: "
                           << Qt::hex << m_Header.magic;

                resetFrameReadState();
                return;
            }

            if (m_Header.version != rdl::core::FRAME_VERSION)
            {
                qWarning() << "NetworkClient: unsupported frame version:"
                           << m_Header.version;
                
                resetFrameReadState();
                return;
            }

            if (m_Header.dataSize == 0)
            {
                qWarning() << "NetworkClient: frame with zero dataSize";

                resetFrameReadState();
                return;
            }

            qInfo() << "NetworkClient: header received:"
                    << " w=" << m_Header.width
                    << ", h=" << m_Header.height
                    << ", fmt=" << m_Header.pixelFormat
                    << ", dataSize=" << m_Header.dataSize;

            m_PayloadBuffer.clear();
            m_PayloadBuffer.reserve(static_cast<int>(m_Header.dataSize));

            m_State = ReadState::Payload;
        }
        else if(m_State == ReadState::Payload)
        {
            const auto totalSize{ m_Header.dataSize };
            const auto bytesReceived{ static_cast<uint32_t>(m_PayloadBuffer.size()) };
            const auto bytesToReceive = totalSize - bytesReceived;

            if (bytesToReceive > 0)
            {
                QByteArray chunk = m_Socket->read(bytesToReceive);
                m_PayloadBuffer.append(chunk);
            }

            if (m_PayloadBuffer.size() < static_cast<int>(totalSize))
                return; // Not all expected data have been received

            qInfo() << "NetworkClient: full frame received: size="
                    << m_PayloadBuffer.size()
                    << " (remaining bytes in socket: " << m_Socket->bytesAvailable() << ")";

            if (m_Header.pixelFormat != static_cast<uint32_t>(rdl::core::FramePixelFormat::BGRA32))
            {
                qWarning() << "NetworkClient: unsupported pixel format="
                           << m_Header.pixelFormat;

                resetFrameReadState();
                return;
            }

            const auto w{ static_cast<int>(m_Header.width) };
            const auto h{ static_cast<int>(m_Header.height) };

            if (m_PayloadBuffer.size() != w * h * 4)
            {
                qWarning() << "NetworkClient: payload size mismatch: "
                           << m_PayloadBuffer.size()
                           << ", expected:" << (w * h * 4);

                resetFrameReadState();
                return;
            }

            FrameJob job;
            job.header = m_Header;
            job.payload = std::move(m_PayloadBuffer);

            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_FrameQueue.emplace_back(std::move(job));
            }

            m_QueueCv.notify_one();

            resetFrameReadState();

            return;
        }
    }
}

/* OPTIMISATION: decouple frame buffer to image
 * conversion to a separate thread to unblock reception
 * of the next frame */
void NetworkClient::workerLoop()
{
    while (true)
    {
        FrameJob job;

        {
            // Locks mutex in c-tor
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            // Unlocks lock in wait immediate when enters cv.wait(),
            // then locks again when predicate becomes true
            m_QueueCv.wait(lock, [this]() {
                return m_StopWorker || !m_FrameQueue.empty();
            });

            if (m_StopWorker && m_FrameQueue.empty())
                break;
            
            job = std::move(m_FrameQueue.front());
            m_FrameQueue.pop_front();
        }

        const int w = static_cast<int>(job.header.width);
        const int h = static_cast<int>(job.header.height);

        // Uses external payload buffer (destroyed at the end of iteration)
        QImage tmp(reinterpret_cast<const uchar*>(job.payload.constData()),
                    w, h,  QImage::Format_ARGB32);

        // frame image uses own payload buffer after copy
        QImage frame = tmp.copy();

        emit frameReceived(frame);
    }
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError)
{
    qWarning() << "NetworkClient: error=" << m_Socket->errorString();
}

void NetworkClient::resetFrameReadState()
{
    m_State = ReadState::Header;
    m_HeaderBuffer.clear();
    m_HeaderBuffer.reserve(static_cast<int>(sizeof(rdl::core::FrameHeader)));
    m_PayloadBuffer.clear();
}