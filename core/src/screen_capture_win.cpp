#include "screen_capture.h"

#include <windows.h>

namespace rdl::core
{
    bool CaptureScreen(
            std::vector<std::byte>& pixels,
            int& width,
            int& height,
            int& bytesPerLine) // bytes per line
    {
        const auto w = GetSystemMetrics(SM_CXSCREEN);
        const auto h = GetSystemMetrics(SM_CYSCREEN);

        if (w <= 0 || h <= 0)
            return false;

        auto hSDC = GetDC(nullptr);
        if (!hSDC)
            return false;

        auto hMDC = CreateCompatibleDC(hSDC);
        if (!hMDC)
        {
            ReleaseDC(nullptr, hSDC);
            return false;
        }

        auto bitmap = CreateCompatibleBitmap(hSDC, w, h);
        if (!bitmap)
        {
            DeleteDC(hMDC);
            ReleaseDC(nullptr, hSDC);
            return false;
        }

        auto oldObj = SelectObject(hMDC, bitmap);

        auto status = BitBlt(hMDC, 0, 0, w, h, hSDC, 0, 0, SRCCOPY|CAPTUREBLT);

        ReleaseDC(nullptr, hSDC);

        if (!status)
        {
            SelectObject(hMDC, oldObj);
            DeleteObject(bitmap);
            DeleteDC(hMDC);
            return false;
        }

        // Specifies the desired format for the DIB data
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h; // top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        const int lineSize{ w * 4 };
        const size_t bufSize{ static_cast<size_t>(lineSize) * static_cast<size_t>(h)};
        pixels.resize(bufSize);

        // pixels will contain RGBA data.
        // The color table should consist of literal red, green, blue (RGB) values.
        //
        // pixels is a valid pointer -> the first six members of the BITMAPINFOHEADER
        // structure must be initialized to specify the size and format of the DIB.
        // The scan lines must be aligned on a DWORD
        int lines = GetDIBits(hMDC, bitmap, 0, static_cast<UINT>(h),
                                pixels.data(), &bmi, DIB_RGB_COLORS);

        SelectObject(hMDC, oldObj);
        DeleteObject(bitmap);
        DeleteDC(hMDC);

        // If the function fails, the return value is zero
        if (!lines)
            return false;
        
        // modify output values on success only
        width = w;
        height = h;
        bytesPerLine = lineSize;

        return true;
    }
}