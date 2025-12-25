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
#include "firmware_handlers_builder.hpp"

#include "file_handler.hpp"
#include "fs.hpp"
#include "general_systemd.hpp"
#include "skip_action.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace ipmi_flash
{
std::vector<HandlerConfig<ActionPack>>
    FirmwareHandlersBuilder::buildHandlerFromJson(const nlohmann::json& data)
{
    std::vector<HandlerConfig<ActionPack>> handlers;

    for (const auto& item : data)
    {
        try
        {
            HandlerConfig<ActionPack> output;

            /* at() throws an exception when the key is not present. */
            item.at("blob").get_to(output.blobId);

            /* name must be: /flash/... */
            if (!std::regex_match(output.blobId, std::regex("^\\/flash\\/.+")))
            {
                throw std::runtime_error(
                    "Invalid blob name: '" + output.blobId +
                    "' must start with /flash/");
            }

            /* handler is required. */
            const auto& h = item.at("handler");
            const std::string handlerType = h.at("type");
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
            std::unique_ptr<ActionPack> pack = std::make_unique<ActionPack>();

            /* to make an action optional, assign type "skip" */
            const auto& prep = a.at("preparation");
            const std::string prepareType = prep.at("type");
            if (prepareType == "systemd")
            {
                pack->preparation = buildSystemd(prep);
            }
            else if (prepareType == "skip")
            {
                pack->preparation = SkipAction::CreateSkipAction();
            }
            else
            {
                throw std::runtime_error(
                    "Invalid preparation type: " + prepareType);
            }

            const auto& verify = a.at("verification");
            const std::string verifyType = verify.at("type");
            if (verifyType == "fileSystemdVerify")
            {
                pack->verification = buildFileSystemd(verify);
            }
            else if (verifyType == "systemd")
            {
                pack->verification = buildSystemd(verify);
            }
            else if (verifyType == "skip")
            {
                pack->verification = SkipAction::CreateSkipAction();
            }
            else
            {
                throw std::runtime_error(
                    "Invalid verification type:" + verifyType);
            }

            const auto& update = a.at("update");
            const std::string updateType = update.at("type");
            if (updateType == "reboot")
            {
                pack->update = SystemdNoFile::CreateSystemdNoFile(
                    sdbusplus::bus::new_default(), "reboot.target",
                    "replace-irreversibly");
            }
            else if (updateType == "fileSystemdUpdate")
            {
                pack->update = buildFileSystemd(update);
            }
            else if (updateType == "systemd")
            {
                pack->update = buildSystemd(update);
            }
            else if (updateType == "skip")
            {
                pack->update = SkipAction::CreateSkipAction();
            }
            else
            {
                throw std::runtime_error("Invalid update type: " + updateType);
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
