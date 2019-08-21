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

#include "prepare_systemd.hpp"

#include "status.hpp"

#include <memory>

namespace ipmi_flash
{

std::unique_ptr<TriggerableActionInterface>
    SystemdPreparation::CreatePreparation(sdbusplus::bus::bus&& bus,
                                          const std::string& service,
                                          const std::string& mode)
{
    return std::make_unique<SystemdPreparation>(std::move(bus), service, mode);
}

bool SystemdPreparation::trigger()
{
    static constexpr auto systemdService = "org.freedesktop.systemd1";
    static constexpr auto systemdRoot = "/org/freedesktop/systemd1";
    static constexpr auto systemdInterface = "org.freedesktop.systemd1.Manager";

    auto method = bus.new_method_call(systemdService, systemdRoot,
                                      systemdInterface, "StartUnit");
    method.append(triggerService);
    method.append(mode);

    try
    {
        bus.call_noreply(method);
    }
    catch (const sdbusplus::exception::SdBusError& ex)
    {
        /* TODO: Once logging supports unit-tests, add a log message to test
         * this failure.
         */
        state = ActionStatus::failed;
        return false;
    }

    state = ActionStatus::success;
    return true;
}

void SystemdPreparation::abort()
{
    return;
}

ActionStatus SystemdPreparation::status()
{
    return state;
}

} // namespace ipmi_flash
