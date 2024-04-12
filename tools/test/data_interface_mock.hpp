#pragma once

#include "interface.hpp"

#include <gmock/gmock.h>

namespace host_tool
{

class DataInterfaceMock : public DataInterface
{
  public:
    virtual ~DataInterfaceMock() = default;

    MOCK_METHOD(bool, sendContents, (const std::string&, std::uint16_t),
                (override));
    MOCK_METHOD(void, waitForRetry, (), (override));
    MOCK_METHOD(ipmi_flash::FirmwareFlags::UpdateFlags, supportedType, (),
                (const, override));
};

} // namespace host_tool
