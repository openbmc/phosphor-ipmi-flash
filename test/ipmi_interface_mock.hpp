#pragma once

#include "ipmi_interface.hpp"

#include <gmock/gmock.h>

namespace host_tool
{

class IpmiInterfaceMock : public IpmiInterface
{
  public:
    virtual ~IpmiInterfaceMock() = default;
    MOCK_METHOD1(sendPacket,
                 std::vector<std::uint8_t>(std::vector<std::uint8_t>&));
};

} // namespace host_tool
