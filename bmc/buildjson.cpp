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

#include <nlohmann/json.hpp>
#include <sdbusplus/bus.hpp>

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

namespace ipmi_flash
{

std::unique_ptr<TriggerableActionInterface>
    buildFileSystemd(const nlohmann::json& data)
{
    /* This type of action requires a path and unit, and optionally a mode. */
    const auto& path = data.at("path");
    const auto& unit = data.at("unit");

    /* the mode parameter is optional. */
    std::string systemdMode = "replace";
    const auto& mode = data.find("mode");
    if (mode != data.end())
    {
        systemdMode = data.at("mode").get<std::string>();
    }

    return SystemdWithStatusFile::CreateSystemdWithStatusFile(
        sdbusplus::bus::new_default(), path, unit, systemdMode);
}

std::unique_ptr<TriggerableActionInterface>
    buildSystemd(const nlohmann::json& data)
{
    /* This type of action requires a unit, and optionally a mode. */
    const auto& unit = data.at("unit");

    /* the mode parameter is optional. */
    std::string systemdMode = "replace";
    const auto& mode = data.find("mode");
    if (mode != data.end())
    {
        systemdMode = data.at("mode").get<std::string>();
    }

    return SystemdNoFile::CreateSystemdNoFile(sdbusplus::bus::new_default(),
                                              unit, systemdMode);
}

template <typename T>
std::vector<HandlerConfig<T>>
    handlersBuilderIfc<T>::buildHandlerConfigs(const std::string& directory)
{
    std::vector<HandlerConfig<T>> output;

    std::vector<std::string> jsonPaths = GetJsonList(directory);

    for (const auto& path : jsonPaths)
    {
        std::ifstream jsonFile(path);
        if (!jsonFile.is_open())
        {
            std::fprintf(stderr, "Unable to open json file: %s\n",
                         path.c_str());
            continue;
        }

        auto data = nlohmann::json::parse(jsonFile, nullptr, false);
        if (data.is_discarded())
        {
            std::fprintf(stderr, "Parsing json failed: %s\n", path.c_str());
        }

        std::vector<HandlerConfig<T>> configs = buildHandlerFromJson(data);
        std::move(configs.begin(), configs.end(), std::back_inserter(output));
    }
    return output;
}

} // namespace ipmi_flash
