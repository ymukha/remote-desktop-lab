#pragma once

#include <cstddef>

namespace rdl::core
{
    class ITransportAdapter
    {
    public:
        virtual ~ITransportAdapter() = default;

        virtual void send(const std::byte* data, size_t size) = 0;
        virtual size_t bytesToWrite() const = 0;
    };
}