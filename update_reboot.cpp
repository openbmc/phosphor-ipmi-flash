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

#include "update_reboot.hpp"

#include "status.hpp"
#include "update.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>

namespace ipmi_flash
{

std::unique_ptr<UpdateInterface>
    RebootUpdateMechanism::CreateRebootUpdate(sdbusplus::bus::bus&& bus)
{
    return std::make_unique<UpdateInterface>(std::move(bus));
}

void RebootUpdateMechanism::abortUpdate()
{
    return;
}

UpdateStatus RebootUpdateMechanism::status()
{
    /* For now, don't check if the target failed. */
    return UpdateStatus::running;
}

} // namespace ipmi_flash
