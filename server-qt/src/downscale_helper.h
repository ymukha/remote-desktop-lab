#pragma once

#include <QImage>
#include <QSize>

#include <vector>
#include <cstddef>

namespace helpers
{
    static void downscaleFrame(const std::vector<std::byte>& inFrame, int inWidth, int inHeight, int inBytesPerLine,
                               std::vector<std::byte>& outFrame, int& outWidth, int& outHeight, int& outBytesPerLine,
                               int maxWidth, int maxHeight)
    {
        if (inWidth <= maxWidth && inHeight <= maxHeight)
        {
            outWidth = inWidth;
            outHeight = inHeight;
            outBytesPerLine = inBytesPerLine;

            const auto size = static_cast<size_t>(inBytesPerLine) * static_cast<size_t>(inHeight);
            outFrame.resize(size);
            std::memcpy(outFrame.data(), inFrame.data(), size);

            return;
        }

        QImage inImage(reinterpret_cast<const uchar*>(inFrame.data()),
                       inWidth, inHeight, inBytesPerLine, QImage::Format_ARGB32);
        QSize targetSize(inWidth, inHeight);

        targetSize.scale(maxWidth, maxHeight, Qt::KeepAspectRatio);

        QImage outImage = inImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        outWidth = outImage.width();
        outHeight = outImage.height();
        outBytesPerLine = outImage.bytesPerLine();

        const auto size = static_cast<size_t>(outBytesPerLine) * static_cast<size_t>(outHeight);
        outFrame.resize(size);
        std::memcpy(outFrame.data(), outImage.bits(), size);
    }
}