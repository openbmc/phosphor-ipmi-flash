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

#include "general_systemd.hpp"

#include "status.hpp"

#include <fstream>
#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>
#include <vector>

namespace ipmi_flash
{

static constexpr auto systemdService = "org.freedesktop.systemd1";
static constexpr auto systemdRoot = "/org/freedesktop/systemd1";
static constexpr auto systemdInterface = "org.freedesktop.systemd1.Manager";

std::unique_ptr<TriggerableActionInterface>
    SystemdNoFile::CreateSystemdNoFile(sdbusplus::bus::bus&& bus,
                                       const std::string& service,
                                       const std::string& mode)
{
    return std::make_unique<SystemdNoFile>(std::move(bus), service, mode);
}

bool SystemdNoFile::trigger()
{
    auto method = bus.new_method_call(systemdService, systemdRoot,
                                      systemdInterface, "StartUnit");
    method.append(triggerService);
    method.append(mode);

    try
    {
        bus.call_noreply(method);
        return true;
    }
    catch (const sdbusplus::exception::SdBusError& ex)
    {
        /* TODO: Once logging supports unit-tests, add a log message to test
         * this failure.
         */
        return false;
    }
}

void SystemdNoFile::abort()
{
    return;
}

ActionStatus SystemdNoFile::status()
{
    auto obj_req = bus.new_method_call(systemdService, systemdRoot,
                                       systemdInterface, "GetUnit");
    obj_req.append(triggerService);

    std::string obj;
    try
    {
        bus.call(obj_req).read(obj);
    }
    catch (const sdbusplus::exception::SdBusError& ex)
    {
        return ActionStatus::unknown;
    }

    auto status_req = bus.new_method_call(
        systemdService, obj.c_str(), "org.freedesktop.DBus.Properties", "Get");
    status_req.append("org.freedesktop.systemd1.Service", "ActiveState");
    std::string status;
    try
    {
        bus.call(status_req).read(status);
    }
    catch (const sdbusplus::exception::SdBusError& ex)
    {
        return ActionStatus::unknown;
    }

    if (status == "activating" || status == "deactivating" ||
        status == "reloading")
    {
        return ActionStatus::running;
    }
    if (status == "active" || status == "inactive")
    {
        return ActionStatus::success;
    }
    if (status == "failed")
    {
        return ActionStatus::failed;
    }
    return ActionStatus::unknown;
}

const std::string& SystemdNoFile::getMode() const
{
    return mode;
}

std::unique_ptr<TriggerableActionInterface>
    SystemdWithStatusFile::CreateSystemdWithStatusFile(
        sdbusplus::bus::bus&& bus, const std::string& path,
        const std::string& service, const std::string& mode)
{
    return std::make_unique<SystemdWithStatusFile>(std::move(bus), path,
                                                   service, mode);
}

ActionStatus SystemdWithStatusFile::status()
{
    ActionStatus result = ActionStatus::unknown;

    std::ifstream ifs;
    ifs.open(checkPath);
    if (ifs.good())
    {
        /*
         * Check for the contents of the file, accepting:
         * running, success, or failed.
         */
        std::string status;
        ifs >> status;
        if (status == "running")
        {
            result = ActionStatus::running;
        }
        else if (status == "success")
        {
            result = ActionStatus::success;
        }
        else if (status == "failed")
        {
            result = ActionStatus::failed;
        }
    }

    return result;
}

} // namespace ipmi_flash
