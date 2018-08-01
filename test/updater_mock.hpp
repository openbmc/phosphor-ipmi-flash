#pragma once

#include <gmock/gmock.h>

#include "flash-ipmi.hpp"

class UpdaterMock : public UpdateInterface
{
  public:
    virtual ~UpdaterMock() = default;

    MOCK_METHOD1(start, bool(uint32_t));
};
