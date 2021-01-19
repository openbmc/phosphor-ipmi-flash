#pragma once

#include "progress.hpp"

#include <cstdint>

#include <gmock/gmock.h>

namespace host_tool
{

class ProgressMock : public ProgressInterface
{
  public:
    MOCK_METHOD(void, updateProgress, (std::int64_t), (override));
    MOCK_METHOD(void, start, (std::int64_t), (override));
    MOCK_METHOD(void, finish, (), (override));
    MOCK_METHOD(void, abort, (), (override));
};

} // namespace host_tool
