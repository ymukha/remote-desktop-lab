#pragma once

#include <cstddef>

#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QIODevice>

#include "frame_codec_interface.h"
#include "frame_protocol.h"

class JPEGFrameCodec : public rdl::core::IFrameCodec
{
public:

    void encode(const std::vector<std::byte>& frame,
                            int width, int height, int bytesPerLine,
                            rdl::core::ITransportAdapter& adapter) override
    {        
        QImage frameImage(reinterpret_cast<const uchar*>(frame.data()),
                 width, height, bytesPerLine,
                 QImage::Format_ARGB32);

        QByteArray jpegData;
        jpegData.reserve(frameImage.width() * frameImage.height() / 2); // roughly

        QBuffer frameBuffer(&jpegData);
        frameBuffer.open(QIODevice::WriteOnly);

        if (!frameImage.save(&frameBuffer, "JPEG",
                                JPEGFrameCodec::FrameQuality))
            return;

        auto header = rdl::core::makeFrameHeader(static_cast<uint32_t>(frameImage.width()),
                                                 static_cast<uint32_t>(frameImage.height()),
                                                 rdl::core::FramePixelFormat::JPEG,
                                                 static_cast<uint32_t>(jpegData.size()));

        auto pendingData = adapter.bytesToWrite();
        auto maxPendingData = JPEGFrameCodec::MaxFrameBacklog * static_cast<size_t>(header.dataSize);

        /* OPTIMISATION: If network quality is poor or client is
        * slow enough to receive frames at server's speed, those
        * will be accumulated, incrementally increase actual
        * displaying latency. Client will receive and
        * display already outdated frames. So, allow only short
        * backlog of pending / non-transmitted frames and
        * decrease latency to actual frame displaying (for
        * the cost of intermediate frames drop) */
        if (pendingData > maxPendingData)
            return;
        
        adapter.send(reinterpret_cast<const std::byte*>(&header), sizeof(header));
        adapter.send(reinterpret_cast<const std::byte*>(jpegData.constData()), jpegData.size());
    }

protected:
    static const size_t MaxFrameBacklog{ 4 }; // Smaller frames, allow longer backlog
    static const int FrameQuality{ 70 };
};