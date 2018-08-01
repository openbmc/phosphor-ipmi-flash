#pragma once

#include <gmock/gmock.h>
#include <vector>

#include "flash-ipmi.hpp"

class UpdaterMock : public UpdateInterface
{
  public:
    virtual ~UpdaterMock() = default;

    MOCK_METHOD1(start, bool(uint32_t));
    MOCK_METHOD2(flashData, bool(uint32_t, const std::vector<uint8_t>&));
};
