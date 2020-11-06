#pragma once

#include "firmware_handler.hpp"
#include "triggerable_mock.hpp"

#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
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
