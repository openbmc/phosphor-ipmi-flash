#pragma once

#include "data_handler.hpp"

#include <gmock/gmock.h>

namespace ipmi_flash
{

class DataHandlerMock : public DataInterface
{
  public:
    virtual ~DataHandlerMock() = default;

    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(std::vector<std::uint8_t>, copyFrom, (std::uint32_t),
                (override));
    MOCK_METHOD(bool, writeMeta, (const std::vector<std::uint8_t>&),
                (override));
    MOCK_METHOD(std::vector<std::uint8_t>, readMeta, (), (override));
};

} // namespace ipmi_flash
