#pragma once

#include "io_interface.hpp"

#include <gmock/gmock.h>

namespace host_tool
{

class HostIoInterfaceMock : public HostIoInterface
{
  public:
    ~HostIoInterfaceMock() = default;

    MOCK_METHOD(bool, read, (const std::size_t, const std::size_t, void* const),
                (override));
    MOCK_METHOD(bool, write,
                (const std::size_t, const std::size_t, const void* const),
                (override));
};

} // namespace host_tool
