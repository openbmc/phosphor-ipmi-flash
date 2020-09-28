#pragma once

#include "firmware_handler.hpp"
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

ActionMap CreateActionMap(const std::string& blobPath)
{
    std::unique_ptr<ActionPack> actionPack = std::make_unique<ActionPack>();
    actionPack->preparation = CreateTriggerMock();
    actionPack->verification = CreateTriggerMock();
    actionPack->update = CreateTriggerMock();

    ActionMap map;
    map[blobPath] = std::move(actionPack);
    return map;
}

} // namespace ipmi_flash
