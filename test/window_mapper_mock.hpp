#pragma once

#include "window_hw_interface.hpp"

#include <cstdint>
#include <utility>
#include <vector>

#include <gmock/gmock.h>

namespace ipmi_flash
{

class HardwareInterfaceMock : public HardwareMapperInterface
{
  public:
    virtual ~HardwareInterfaceMock() = default;

    MOCK_METHOD0(close, ());
    MOCK_METHOD2(mapWindow,
                 std::pair<std::uint32_t, std::uint32_t>(std::uint32_t,
                                                         std::uint32_t));
    MOCK_METHOD1(copyFrom, std::vector<std::uint8_t>(std::uint32_t));
};

} // namespace ipmi_flash
