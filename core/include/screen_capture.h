#pragma once

#include <vector>
#include <cstddef>

namespace rdl::core
{
    bool CaptureScreen(
        std::vector<std::byte>& pixels,
        int& width,
        int& height,
        int& bytesPerLine);
}