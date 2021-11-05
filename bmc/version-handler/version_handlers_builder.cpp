/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "version_handlers_builder.hpp"

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

std::vector<HandlerConfig<VersionBlobHandler::ActionPack>>
    VersionHandlersBuilder::buildHandlerFromJson(const nlohmann::json& data)
{
    std::vector<HandlerConfig<VersionBlobHandler::ActionPack>> handlers;

    for (const auto& item : data)
    {
        HandlerConfig<VersionBlobHandler::ActionPack> output;

        if (!item.contains("blob"))
        {
            continue;
        }
        item.at("blob").get_to(output.blobId);

        /* name must be: /flash/... or /version/...*/
        std::regex regexpr("^\\/(?:flash|version)\\/(.+)");
        std::smatch matches;
        if (!std::regex_search(output.blobId, matches, regexpr))
        {
            std::fprintf(stderr,
                         "Excepted building HandlerConfig from json: Invalid "
                         "blob name: '%s' must start with /flash/ or /version/",
                         output.blobId.c_str());
        }
        output.blobId = "/version/" + matches[1].str();

        if (!item.contains("version"))
        {
            continue;
        }
        /* version is required. */
        const auto& v = item.at("version");
        /* version must have handler */
        if (!v.contains("handler"))
        {
            continue;
        }
        const auto& h = v.at("handler");

        const std::string& handlerType = h.at("type");
        if (handlerType == "file")
        {
            const auto& path = h.at("path");
            output.handler = std::make_unique<FileHandler>(path);
        }
        else
        {
            std::fprintf(stderr,
                         "Excepted building HandlerConfig from json: Invalid "
                         "handler type: %s",
                         handlerType.c_str());
        }

        /* actions are required (presently). */
        if (!v.contains("actions"))
        {
            continue;
        }
        const auto& a = v.at("actions");
        auto pack = std::make_unique<VersionBlobHandler::ActionPack>();

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
            std::fprintf(stderr,
                         "Excepted building HandlerConfig from json: Invalid "
                         "preparation type: %s\n",
                         onOpenType.c_str());
        }

        output.actions = std::move(pack);
        handlers.push_back(std::move(output));
    }

    return handlers;
}

} // namespace ipmi_flash
