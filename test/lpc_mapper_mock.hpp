#pragma once

#include "window_hw_interface.hpp"

#include <gmock/gmock.h>

namespace blobs
{

class HardwareInterfaceMock : public HardwareMapperInterface
{
  public:
    virtual ~HardwareInterfaceMock() = default;

    MOCK_METHOD2(mapWindow,
                 std::pair<std::uint32_t, std::uint32_t>(std::uint32_t,
                                                         std::uint32_t));
    MOCK_METHOD1(copyFrom, std::vector<std::uint8_t>(std::uint32_t));
};

} // namespace blobs
