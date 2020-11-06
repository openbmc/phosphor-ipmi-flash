/*
 * Copyright 2018 Google Inc.
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

#include "config.h"

#include "file_handler.hpp"
#include "general_systemd.hpp"
#include "image_handler.hpp"
#include "status.hpp"
#include "version_handler.hpp"
#include "version_handlers_builder.hpp"

#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ipmi_flash
{

namespace
{

static constexpr const char* jsonConfigurationPath =
    "/usr/share/phosphor-ipmi-flash/";
} // namespace

std::unique_ptr<blobs::GenericBlobInterface>
    createHandlerFromJsons(VersionHandlersBuilder& builder,
                           const char* configPath)
{
    std::vector<HandlerConfig<VersionActionPack>> configsFromJson =
        builder.buildHandlerConfigs(configPath);

    VersionInfoMap handlerMap;
    for (auto& config : configsFromJson)
    {
        auto [it, inserted] = handlerMap.try_emplace(
            config.blobId, config.blobId, std::move(config.actions),
            std::move(config.handler));
        if (inserted == false)
        {
            std::fprintf(stderr, "duplicate blob id %s, discarding\n",
                         config.blobId.c_str());
        }
        else
        {
            std::fprintf(stderr, "config loaded: %s\n", config.blobId.c_str());
        }
    }
    auto handler = VersionBlobHandler::create(std::move(handlerMap));
    if (!handler)
    {
        std::fprintf(stderr, "Version Handler has an invalid configuration");
        return nullptr;
    }

    return handler;
}
} // namespace ipmi_flash
extern "C"
{
    std::unique_ptr<blobs::GenericBlobInterface> createHandler();
}

std::unique_ptr<blobs::GenericBlobInterface> createHandler()
{
    ipmi_flash::VersionHandlersBuilder builder;
    return ipmi_flash::createHandlerFromJsons(
        builder, ipmi_flash::jsonConfigurationPath);
}
