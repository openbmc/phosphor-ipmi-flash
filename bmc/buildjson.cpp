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
#include "prepare_systemd.hpp"
#include "update_systemd.hpp"
#include "verify_systemd.hpp"

#include <nlohmann/json.hpp>
#include <sdbusplus/bus.hpp>

namespace ipmi_flash
{

HandlerConfig buildHandlerFromJson(const nlohmann::json& data)
{
    HandlerConfig output;

    /* Start only supporting one entry in the list. */
    for (const auto& item : data)
    {
        /* at() throws an exception when the key is not present. */
        item.at("blob").get_to(output.blobId);

        /* handler is required. */
        const auto& h = item.at("handler");
        if (h.at("type") == "file")
        {
            const auto& path = h.at("path");
            output.handler = std::make_unique<FileHandler>(path);
        }

        /* actions are required (presently). */
        const auto& a = item.at("actions");
        std::unique_ptr<ActionPack> pack = std::make_unique<ActionPack>();

        /* It hasn't been fully determined if any action being optional is
         * useful, so for now they will be required.
         * TODO: Evaluate how the behaviors change if some actions are missing,
         * does the code just assume it was successful?  I would think not.
         */
        const auto& prep = a.at("preparation");
        if (prep.at("type") == "systemdPrepare")
        {
            const auto& unit = prep.at("unit");
            pack->preparation = SystemdPreparation::CreatePreparation(
                sdbusplus::bus::new_default(), unit);
        }

        const auto& verify = a.at("verification");
        if (verify.at("type") == "fileSystemdVerify")
        {
            const auto& path = verify.at("path");
            const auto& unit = verify.at("unit");
            pack->verification = SystemdVerification::CreateVerification(
                sdbusplus::bus::new_default(), path, unit);
        }

        const auto& update = a.at("update");
        if (update.at("type") == "reboot")
        {
            static constexpr auto rebootTarget = "reboot.target";
            static constexpr auto rebootMode = "replace-irreversibly";
            pack->update = SystemdUpdateMechanism::CreateSystemdUpdate(
                sdbusplus::bus::new_default(), rebootTarget, rebootMode);
        }

        output.actions = std::move(pack);
    }

    return output;
}

} // namespace ipmi_flash
