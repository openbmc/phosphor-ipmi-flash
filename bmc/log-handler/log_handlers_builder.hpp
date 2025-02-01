// Copyright 2021 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "buildjson.hpp"
#include "log_handler.hpp"

#include <nlohmann/json.hpp>

#include <vector>

namespace ipmi_flash
{
/**
 * provide the method to parse and validate blob entries from json and produce
 * something that is usable by the log handler.
 */
class LogHandlersBuilder : public HandlersBuilderIfc<LogBlobHandler::ActionPack>
{
  public:
    std::vector<HandlerConfig<LogBlobHandler::ActionPack>> buildHandlerFromJson(
        const nlohmann::json& data) override;
};
} // namespace ipmi_flash
