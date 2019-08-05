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
#include "buildjson.hpp"

#include "file_handler.hpp"
#include "fs.hpp"
#include "prepare_systemd.hpp"
#include "update_systemd.hpp"
#include "verify_systemd.hpp"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <nlohmann/json.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <string>
#include <vector>

namespace ipmi_flash
{

std::vector<HandlerConfig> buildHandlerFromJson(const nlohmann::json& data)
{
    std::vector<HandlerConfig> handlers;

    for (const auto& item : data)
    {
        try
        {
            HandlerConfig output;

            /* at() throws an exception when the key is not present. */
            item.at("blob").get_to(output.blobId);

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
                throw std::runtime_error("Invalid handler type: " +
                                         handlerType);
            }

            /* actions are required (presently). */
            const auto& a = item.at("actions");
            std::unique_ptr<ActionPack> pack = std::make_unique<ActionPack>();

            /* It hasn't been fully determined if any action being optional is
             * useful, so for now they will be required.
             * TODO: Evaluate how the behaviors change if some actions are
             * missing, does the code just assume it was successful?  I would
             * think not.
             */
            const auto& prep = a.at("preparation");
            const std::string prepareType = prep.at("type");
            if (prepareType == "systemd")
            {
                const auto& unit = prep.at("unit");
                pack->preparation = SystemdPreparation::CreatePreparation(
                    sdbusplus::bus::new_default(), unit);
            }
            else
            {
                throw std::runtime_error("Invalid preparation type: " +
                                         prepareType);
            }

            const auto& verify = a.at("verification");
            const std::string verifyType = verify.at("type");
            if (verifyType == "fileSystemdVerify")
            {
                const auto& path = verify.at("path");
                const auto& unit = verify.at("unit");

                /* the mode parameter is optional. */
                std::string systemdMode = "replace";
                const auto& mode = verify.find("mode");
                if (mode != verify.end())
                {
                    systemdMode = verify.at("mode").get<std::string>();
                }

                pack->verification = SystemdVerification::CreateVerification(
                    sdbusplus::bus::new_default(), path, unit, systemdMode);
            }
            else
            {
                throw std::runtime_error("Invalid verification type:" +
                                         verifyType);
            }

            const auto& update = a.at("update");
            const std::string updateType = update.at("type");
            if (updateType == "reboot")
            {
                static constexpr auto rebootTarget = "reboot.target";
                static constexpr auto rebootMode = "replace-irreversibly";
                pack->update = SystemdUpdateMechanism::CreateSystemdUpdate(
                    sdbusplus::bus::new_default(), rebootTarget, rebootMode);
            }
            else if (updateType == "systemd")
            {
                const auto& unit = update.at("unit");

                /* the mode parameter is optional. */
                std::string systemdMode = "replace";
                const auto& mode = update.find("mode");
                if (mode != update.end())
                {
                    systemdMode = update.at("mode").get<std::string>();
                }

                pack->update = SystemdUpdateMechanism::CreateSystemdUpdate(
                    sdbusplus::bus::new_default(), unit, systemdMode);
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

std::vector<HandlerConfig> BuildHandlerConfigs(const std::string& directory)
{
    using namespace phosphor::logging;

    std::vector<HandlerConfig> output;

    std::vector<std::string> jsonPaths = GetJsonList(directory);

    for (const auto& path : jsonPaths)
    {
        std::ifstream jsonFile(path);
        if (!jsonFile.is_open())
        {
            log<level::ERR>("Unable to open json file",
                            entry("PATH=%s", path.c_str()));
            continue;
        }

        auto data = nlohmann::json::parse(jsonFile, nullptr, false);
        if (data.is_discarded())
        {
            log<level::ERR>("Parsing json failed",
                            entry("PATH=%s", path.c_str()));
            continue;
        }

        std::vector<HandlerConfig> configs = buildHandlerFromJson(data);
        std::move(configs.begin(), configs.end(), std::back_inserter(output));
    }

    return output;
}

} // namespace ipmi_flash
