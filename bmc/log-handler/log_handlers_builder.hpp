#pragma once
#include <nlohmann/json.hpp>
#include <vector>

#include "buildjson.hpp"
#include "log_handler.hpp"

namespace ipmi_flash {
/**
 * provide the method to parse and validate blob entries from json and produce
 * something that is usable by the log handler.
 */
class LogHandlersBuilder
    : public HandlersBuilderIfc<LogBlobHandler::ActionPack> {
 public:
  std::vector<HandlerConfig<LogBlobHandler::ActionPack>> buildHandlerFromJson(
      const nlohmann::json& data) override;
};
}  // namespace ipmi_flash
