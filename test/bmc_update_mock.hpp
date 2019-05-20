#pragma once

#include "status.hpp"
#include "verify.hpp"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{

class UpdateMock : public UpdateInterface
{
    public:
      MOCK_METHOD0(triggerUpdate, bool());
      MOCK_METHOD0(abortUpdate, void());
      MOCK_METHOD0(status, UpdateStatus());
};

}
