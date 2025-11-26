#pragma once

#include <cstddef>
#include <vector>

#include "transport_adapter_interface.h"

namespace rdl::core
{
    class IFrameCodec
    {
    public:
        virtual ~IFrameCodec() = default;

        virtual void encode(const std::vector<std::byte>& frame,
                            int width, int height, int bytesPerLine,
                            ITransportAdapter& adapter) = 0;
    };
}