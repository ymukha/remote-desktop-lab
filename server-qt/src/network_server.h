#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <vector>
#include <cstddef>

#include "transport_adapter_interface.h"
#include "frame_codec_interface.h"

class NetworkServer : public QObject
                    , protected rdl::core::ITransportAdapter
{
    Q_OBJECT

public:
    explicit NetworkServer(rdl::core::IFrameCodec& codec,
                           QObject* parent = nullptr);

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

    // ITransportAdapter
    void send(const std::byte* data, size_t size) override;
    size_t bytesToWrite() const override;

    void sendFrameRaw(const std::vector<std::byte>& pixels,
                   int width, int height, int bytesPerLine);

    static const quint64 MaxFrameBacklog{ 2 };

    QTcpServer *m_Server{nullptr};
    QTcpSocket *m_ClientSocket{nullptr};

    rdl::core::IFrameCodec& m_Codec;
};