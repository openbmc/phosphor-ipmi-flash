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
    MOCK_METHOD(bool, trigger, (), (override));
    MOCK_METHOD(void, abort, (), (override));
    MOCK_METHOD(ActionStatus, status, (), (override));

    using TriggerableActionInterface::cb;
};

std::unique_ptr<TriggerableActionInterface> CreateTriggerMock();

} // namespace ipmi_flash
