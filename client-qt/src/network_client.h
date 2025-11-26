#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QImage>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

#include "frame_protocol.h"

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject* parent = nullptr);
    ~NetworkClient();

    void connectToServer(const QString& host, quint16 port);

signals:
    void frameReceived(const QImage& frame);

private slots:
    void onServerConnect();
    void onServerDisconnect();
    void onReadFromServer();
    void onSocketError(QAbstractSocket::SocketError);

protected:
    enum class ReadState
    {
        Header,
        Payload,
    };

    struct FrameJob
    {
        rdl::core::FrameHeader header;
        QByteArray payload;
    };

    void resetFrameReadState();

    void workerLoop();

    QTcpSocket *m_Socket{ nullptr };
    ReadState   m_State{ ReadState::Header };
    QByteArray m_HeaderBuffer;
    QByteArray m_PayloadBuffer;
    rdl::core::FrameHeader m_Header{};
    QImage m_FrameImage;

    std::thread m_Worker;
    std::mutex m_QueueMutex;
    std::condition_variable m_QueueCv;
    std::deque<FrameJob> m_FrameQueue;
    bool m_StopWorker{ false };
};