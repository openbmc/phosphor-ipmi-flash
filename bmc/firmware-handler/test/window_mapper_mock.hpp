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

    MOCK_METHOD(MemorySet, open, (), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(WindowMapResult, mapWindow, (std::uint32_t, std::uint32_t),
                (override));
};

} // namespace ipmi_flash
