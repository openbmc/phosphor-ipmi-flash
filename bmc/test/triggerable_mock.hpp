#pragma once

#include "status.hpp"

#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
// TriggerableActionInterface

class TriggerMock : public TriggerableActionInterface
{
  public:
    MOCK_METHOD0(trigger, bool());
    MOCK_METHOD0(abort, void());
    MOCK_METHOD0(status, ActionStatus());
};

std::unique_ptr<TriggerableActionInterface> CreateTriggerMock()
{
    return std::make_unique<TriggerMock>();
}

} // namespace ipmi_flash
