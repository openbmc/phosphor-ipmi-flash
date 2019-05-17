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

#include "verify.hpp"

#include <string>
#include <vector>

namespace {
static constexpr auto systemdService = "org.freedesktop.systemd1";
static constexpr auto systemdRoot = "/org/freedesktop/systemd1";
static constexpr auto systemdInterface = "org.freedesktop.systemd1.Manager";
}

namespace blobs
{

std::unique_ptr<VerificationInterface> Verification::CreateDefaultVerification(sdbusplus::bus::bus&& bus, const std::string& path, const std::string& service)
{
    return std::make_unique<Verification>(std::move(bus), path, service);
}

bool Verification::triggerVerification()
{
    auto method = bus.new_method_call(systemdService, systemdRoot,
                                      systemdInterface, "StartUnit");
    method.append(triggerService);
    method.append("replace");

    try
    {
        bus.call_noreply(method);
    }
    catch (const sdbusplus::exception::SdBusError& ex)
    {
        /* TODO: Once logging supports unit-tests, add a log message to test
         * this failure.
         */
        return false;
    }

    return true;
}

void Verification::abortVerification()
{
    /* TODO: Implement this. */
}

}
