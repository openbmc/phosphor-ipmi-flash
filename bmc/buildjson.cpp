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
#include "general_systemd.hpp"

#include <nlohmann/json.hpp>
#include <sdbusplus/bus.hpp>

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <string>

namespace ipmi_flash
{

std::unique_ptr<TriggerableActionInterface> buildFileSystemd(
    const nlohmann::json& data)
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

std::unique_ptr<TriggerableActionInterface> buildSystemd(
    const nlohmann::json& data)
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

} // namespace ipmi_flash
