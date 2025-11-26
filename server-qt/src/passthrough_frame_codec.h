#pragma once

#include <cstddef>

#include "frame_codec_interface.h"
#include "frame_protocol.h"

class PassthroughFrameCodec : public rdl::core::IFrameCodec
{
public:

    void encode(const std::vector<std::byte>& frame,
                            int width, int height, int bytesPerLine,
                            rdl::core::ITransportAdapter& adapter) override
    {
        auto header = rdl::core::makeFrameHeader(width, height,
                                             rdl::core::FramePixelFormat::BGRA32,
                                             bytesPerLine * height);

        auto pendingData = adapter.bytesToWrite();
        auto maxPendingData = PassthroughFrameCodec::MaxFrameBacklog * static_cast<size_t>(header.dataSize);

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
        adapter.send(frame.data(), bytesPerLine * height);
    }

protected:
    static const size_t MaxFrameBacklog{ 2 };
};