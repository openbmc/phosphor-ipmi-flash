#pragma once
#include "buildjson.hpp"
#include "version_handler.hpp"

#include <nlohmann/json.hpp>

#include <vector>

namespace ipmi_flash
{
/**
 * provide the method to parse and validate blob entries from json and produce
 * something that is usable by the version handler.
 */
class VersionHandlersBuilder : public HandlersBuilderIfc<VersionActionPack>
{
  public:
    std::vector<HandlerConfig<VersionActionPack>>
        buildHandlerFromJson(const nlohmann::json& data) override;
};
} // namespace ipmi_flash
