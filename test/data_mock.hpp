#pragma once

#include "data_handler.hpp"

#include <gmock/gmock.h>

namespace blobs
{

class DataHandlerMock : public DataInterface
{
  public:
    virtual ~DataHandlerMock() = default;

    MOCK_METHOD1(copyFrom, std::vector<std::uint8_t>(std::uint32_t));
};

} // namespace blobs
