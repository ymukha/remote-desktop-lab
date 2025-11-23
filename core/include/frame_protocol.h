#pragma once

#include <cstdint>

namespace rdl::core
{
    constexpr uint32_t FRAME_MAGIC{ 0x52444c46u };
    constexpr uint16_t FRAME_VERSION{ 1 };

    enum class FramePixelFormat : uint32_t
    {
        BGRA32 = 1,
        JPEG = 2,
        PNG = 3,
    };

    /* In principle it's possible to use
     * better format (for ex., tlv), but in this
     * case it will require codecs layer for msg
     * serialization. Looks redundant latency
     * critical path */
    struct FrameHeader
    {
        uint32_t magic; // 4b alignment
        uint16_t version; // 2b alignment -> no padding
        uint16_t headerSize; // 2b alignment -> no padding
        uint32_t width; // 4b alignment -> no padding
        uint32_t height; // 4b alignment -> no padding
        uint32_t pixelFormat; // 4b alignment -> no padding
        uint32_t dataSize; // 4b alignment -> no padding
    }; // max alignment is 4b -> overall struct alignment is 4b
       //   -> no struct padding (packing is not required)

    // Make sure nothing wents wrong at some point
    static_assert(sizeof(FrameHeader) == 24, "FrameHeader size must be 24 bytes");

    inline FrameHeader makeFrameHeader(uint32_t width, uint32_t height, FramePixelFormat format, uint32_t dataSize)
    {
        return {
            FRAME_MAGIC,
            FRAME_VERSION,
            static_cast<uint16_t>(sizeof(FrameHeader)),
            width, height,
            static_cast<uint32_t>(format),
            dataSize
        };
    }
}