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

#include "log_handlers_builder.hpp"

#include "file_handler.hpp"
#include "fs.hpp"
#include "skip_action.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

namespace ipmi_flash
{

std::vector<HandlerConfig<LogBlobHandler::ActionPack>>
    LogHandlersBuilder::buildHandlerFromJson(const nlohmann::json& data)
{
    std::vector<HandlerConfig<LogBlobHandler::ActionPack>> handlers;

    for (const auto& item : data)
    {
        try
        {
            HandlerConfig<LogBlobHandler::ActionPack> output;

            /* at() throws an exception when the key is not present. */
            item.at("blob").get_to(output.blobId);

            /* name must be: /log/ */
            std::regex regexpr("^\\/(?:log)\\/(.+)");
            std::smatch matches;
            if (!std::regex_search(output.blobId, matches, regexpr))
            {
                continue;
            }
            /* log must have handler */
            const auto& h = item.at("handler");

            const std::string& handlerType = h.at("type");
            if (handlerType == "file")
            {
                const auto& path = h.at("path");
                output.handler = std::make_unique<FileHandler>(path);
            }
            else
            {
                throw std::runtime_error(
                    "Invalid handler type: " + handlerType);
            }

            /* actions are required (presently). */
            const auto& a = item.at("actions");
            auto pack = std::make_unique<LogBlobHandler::ActionPack>();

            /* to make an action optional, assign type "skip" */
            const auto& onOpen = a.at("open");
            const std::string& onOpenType = onOpen.at("type");
            if (onOpenType == "systemd")
            {
                pack->onOpen = std::move(buildSystemd(onOpen));
            }
            else if (onOpenType == "skip")
            {
                pack->onOpen = SkipAction::CreateSkipAction();
            }
            else
            {
                throw std::runtime_error(
                    "Invalid preparation type: " + onOpenType);
            }

            const auto& onDelete = a.at("delete");
            const std::string& onDeleteType = onDelete.at("type");
            if (onDeleteType == "systemd")
            {
                pack->onDelete = std::move(buildSystemd(onDelete));
            }
            else if (onDeleteType == "skip")
            {
                pack->onDelete = SkipAction::CreateSkipAction();
            }
            else
            {
                throw std::runtime_error(
                    "Invalid preparation type: " + onDeleteType);
            }

            output.actions = std::move(pack);
            handlers.push_back(std::move(output));
        }
        catch (const std::exception& e)
        {
            /* TODO: Once phosphor-logging supports unit-test injection, fix
             * this to log.
             */
            std::fprintf(stderr,
                         "Excepted building HandlerConfig from json: %s\n",
                         e.what());
        }
    }

    return handlers;
}

} // namespace ipmi_flash
