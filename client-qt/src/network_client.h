#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QImage>

#include "frame_protocol.h"

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject* parent = nullptr);

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

    void resetFrameReadState();

    QTcpSocket *m_Socket{ nullptr };
    ReadState   m_State{ ReadState::Header };
    QByteArray m_HeaderBuffer;
    QByteArray m_PayloadBuffer;
    rdl::core::FrameHeader m_Header{};
    QImage m_FrameImage;
};