#pragma once

#include "status.hpp"
#include "update.hpp"

#include <memory>
#include <vector>

#include <gmock/gmock.h>

namespace ipmi_flash
{

class UpdateMock : public UpdateInterface
{
  public:
    MOCK_METHOD0(triggerUpdate, bool());
    MOCK_METHOD0(abortUpdate, void());
    MOCK_METHOD0(status, ActionStatus());
};

std::unique_ptr<UpdateInterface> CreateUpdateMock()
{
    return std::make_unique<UpdateMock>();
}

} // namespace ipmi_flash
