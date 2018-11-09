#pragma once

#include "data_handler.hpp"

#include <gmock/gmock.h>

namespace blobs
{

class DataHandlerMock : public DataInterface
{
  public:
    virtual ~DataHandlerMock() = default;

    MOCK_METHOD0(init, bool());
    MOCK_METHOD1(copyFrom, std::vector<std::uint8_t>(std::uint32_t));
    MOCK_METHOD1(write, bool(const std::vector<std::uint8_t>&));
    MOCK_METHOD0(read, std::vector<std::uint8_t>());
};

} // namespace blobs
