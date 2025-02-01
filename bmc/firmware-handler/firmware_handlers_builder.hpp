#pragma once
#include "buildjson.hpp"
#include "firmware_handler.hpp"

#include <nlohmann/json.hpp>

#include <vector>

namespace ipmi_flash
{
class FirmwareHandlersBuilder : public HandlersBuilderIfc<ActionPack>
{
  public:
    std::vector<HandlerConfig<ActionPack>> buildHandlerFromJson(
        const nlohmann::json& data) override;
};
} // namespace ipmi_flash
