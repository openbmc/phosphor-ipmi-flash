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
#include "firmware_handler.hpp"
#include "firmware_handlers_builder.hpp"
#include "flags.hpp"
#include "general_systemd.hpp"
#include "image_handler.hpp"
#include "lpc_aspeed.hpp"
#include "lpc_handler.hpp"
#include "lpc_nuvoton.hpp"
#include "net_handler.hpp"
#include "pci_handler.hpp"
#include "status.hpp"
#include "util.hpp"

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

/**
 * Given a name and path, create a HandlerPack.
 *
 * @param[in] name - the blob id path for this
 * @param[in] path - the file path to write the contents.
 * @return the HandlerPack.
 */
HandlerPack CreateFileHandlerPack(const std::string& name,
                                  const std::string& path)
{
    return HandlerPack(name, std::make_unique<FileHandler>(path));
}

#ifdef NUVOTON_P2A_MBOX
static constexpr std::size_t memoryRegionSize = 16 * 1024UL;
#elif defined NUVOTON_P2A_VGA
static constexpr std::size_t memoryRegionSize = 4 * 1024 * 1024UL;
#else
/* The maximum external buffer size we expect is 64KB. */
static constexpr std::size_t memoryRegionSize = 64 * 1024UL;
#endif

} // namespace
} // namespace ipmi_flash

extern "C"
{
std::unique_ptr<blobs::GenericBlobInterface> createHandler();
}

std::unique_ptr<blobs::GenericBlobInterface> createHandler()
{
    using namespace ipmi_flash;

    std::vector<DataHandlerPack> supportedTransports;

    supportedTransports.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

#ifdef ENABLE_PCI_BRIDGE
    supportedTransports.emplace_back(
        FirmwareFlags::UpdateFlags::p2a,
        std::make_unique<PciDataHandler>(MAPPED_ADDRESS, memoryRegionSize));
#endif

#ifdef ENABLE_LPC_BRIDGE
#if defined(ASPEED_LPC)
    supportedTransports.emplace_back(
        FirmwareFlags::UpdateFlags::lpc,
        std::make_unique<LpcDataHandler>(LpcMapperAspeed::createAspeedMapper(
            MAPPED_ADDRESS, memoryRegionSize)));
#elif defined(NUVOTON_LPC)
    supportedTransports.emplace_back(
        FirmwareFlags::UpdateFlags::lpc,
        std::make_unique<LpcDataHandler>(LpcMapperNuvoton::createNuvotonMapper(
            MAPPED_ADDRESS, memoryRegionSize)));
#else
#error "You must specify a hardware implementation."
#endif
#endif

#ifdef ENABLE_NET_BRIDGE
    supportedTransports.emplace_back(FirmwareFlags::UpdateFlags::net,
                                     std::make_unique<NetDataHandler>());
#endif

    ActionMap actionPacks = {};
    FirmwareHandlersBuilder builder;

    auto configsFromJson = builder.buildHandlerConfigsFromDefaultPaths();

    std::vector<HandlerPack> supportedFirmware;

    supportedFirmware.push_back(
        CreateFileHandlerPack(hashBlobId, HASH_FILENAME));

    for (auto& config : configsFromJson)
    {
        supportedFirmware.emplace_back(config.blobId,
                                       std::move(config.handler));
        actionPacks[config.blobId] = std::move(config.actions);

        std::fprintf(stderr, "config loaded: %s\n", config.blobId.c_str());
    }

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(supportedFirmware), std::move(supportedTransports),
        std::move(actionPacks));

    if (!handler)
    {
        std::fprintf(stderr, "Firmware Handler has an invalid configuration");
        return nullptr;
    }

    return handler;
}
