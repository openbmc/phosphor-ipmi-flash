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

#include "buildjson.hpp"
#include "file_handler.hpp"
#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_handler.hpp"
#include "lpc_aspeed.hpp"
#include "lpc_handler.hpp"
#include "lpc_nuvoton.hpp"
#include "pci_handler.hpp"
#include "prepare_systemd.hpp"
#include "status.hpp"
#include "update_systemd.hpp"
#include "util.hpp"
#include "verify_systemd.hpp"

#include <cstdint>
#include <memory>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ipmi_flash
{

namespace
{

/* The maximum external buffer size we expect is 64KB. */
static constexpr std::size_t memoryRegionSize = 64 * 1024UL;

static constexpr const char* jsonConfigurationPath =
    "/usr/share/phosphor-ipmi-flash/";

#ifdef ENABLE_LPC_BRIDGE
#if defined(ASPEED_LPC)
LpcDataHandler lpcDataHandler(
    LpcMapperAspeed::createAspeedMapper(MAPPED_ADDRESS, memoryRegionSize));
#elif defined(NUVOTON_LPC)
LpcDataHandler lpcDataHandler(
    LpcMapperNuvoton::createNuvotonMapper(MAPPED_ADDRESS, memoryRegionSize));
#else
#error "You must specify a hardware implementation."
#endif
#endif

#ifdef ENABLE_PCI_BRIDGE
#if defined(ASPEED_P2A)
PciDataHandler pciDataHandler(MAPPED_ADDRESS, memoryRegionSize);
#else
#error "You must specify a hardware implementation."
#endif
#endif

std::vector<DataHandlerPack> supportedTransports = {
    {FirmwareFlags::UpdateFlags::ipmi, nullptr},
#ifdef ENABLE_PCI_BRIDGE
    {FirmwareFlags::UpdateFlags::p2a, &pciDataHandler},
#endif
#ifdef ENABLE_LPC_BRIDGE
    {FirmwareFlags::UpdateFlags::lpc, &lpcDataHandler},
#endif
};

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

} // namespace
} // namespace ipmi_flash

extern "C" {
std::unique_ptr<blobs::GenericBlobInterface> createHandler();
}

std::unique_ptr<blobs::GenericBlobInterface> createHandler()
{
    /* NOTE: This is unused presently. */
    {
        std::vector<ipmi_flash::HandlerConfig> configsFromJson =
            ipmi_flash::BuildHandlerConfigs(ipmi_flash::jsonConfigurationPath);

        for (const auto& config : configsFromJson)
        {
            std::fprintf(stderr, "config loaded: %s\n", config.blobId.c_str());
        }
    }

    ipmi_flash::ActionMap actionPacks = {};

    {
        auto bmcPack = std::make_unique<ipmi_flash::ActionPack>();

#ifdef ENABLE_REBOOT_UPDATE
        static constexpr auto rebootTarget = "reboot.target";
        static constexpr auto rebootMode = "replace-irreversibly";

        bmcPack->update =
            ipmi_flash::SystemdUpdateMechanism::CreateSystemdUpdate(
                sdbusplus::bus::new_default(), rebootTarget, rebootMode);
#else
        bmcPack->update =
            ipmi_flash::SystemdUpdateMechanism::CreateSystemdUpdate(
                sdbusplus::bus::new_default(), UPDATE_DBUS_SERVICE);
#endif

        bmcPack->preparation =
            ipmi_flash::SystemdPreparation::CreatePreparation(
                sdbusplus::bus::new_default(), PREPARATION_DBUS_SERVICE);

        bmcPack->verification =
            ipmi_flash::SystemdVerification::CreateVerification(
                sdbusplus::bus::new_default(), VERIFY_STATUS_FILENAME,
                VERIFY_DBUS_SERVICE);

        std::string bmcName;
#ifdef ENABLE_STATIC_LAYOUT
        bmcName = ipmi_flash::staticLayoutBlobId;
#endif
#ifdef ENABLE_TARBALL_UBI
        bmcName = ipmi_flash::ubiTarballBlobId;
#endif

        actionPacks[bmcName] = std::move(bmcPack);
    }

#ifdef ENABLE_HOST_BIOS
    {
        auto biosPack = std::make_unique<ipmi_flash::ActionPack>();

        biosPack->preparation =
            ipmi_flash::SystemdPreparation::CreatePreparation(
                sdbusplus::bus::new_default(), PREPARATION_BIOS_TARGET);

        biosPack->verification =
            ipmi_flash::SystemdVerification::CreateVerification(
                sdbusplus::bus::new_default(), BIOS_VERIFY_STATUS_FILENAME,
                VERIFY_BIOS_TARGET);

        biosPack->update =
            ipmi_flash::SystemdUpdateMechanism::CreateSystemdUpdate(
                sdbusplus::bus::new_default(), UPDATE_BIOS_TARGET);

        actionPacks[ipmi_flash::biosBlobId] = std::move(biosPack);
    }
#endif

    std::vector<ipmi_flash::HandlerPack> supportedFirmware;

    supportedFirmware.push_back(std::move(ipmi_flash::CreateFileHandlerPack(
        ipmi_flash::hashBlobId, HASH_FILENAME)));

#ifdef ENABLE_STATIC_LAYOUT
    supportedFirmware.push_back(std::move(ipmi_flash::CreateFileHandlerPack(
        ipmi_flash::staticLayoutBlobId, STATIC_HANDLER_STAGED_NAME)));
#endif
#ifdef ENABLE_TARBALL_UBI
    supportedFirmware.push_back(std::move(ipmi_flash::CreateFileHandlerPack(
        ipmi_flash::ubiTarballBlobId, TARBALL_STAGED_NAME)));
#endif
#ifdef ENABLE_HOST_BIOS
    supportedFirmware.push_back(std::move(ipmi_flash::CreateFileHandlerPack(
        ipmi_flash::biosBlobId, BIOS_STAGED_NAME)));
#endif

    auto handler = ipmi_flash::FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(supportedFirmware), ipmi_flash::supportedTransports,
        std::move(actionPacks));

    if (!handler)
    {
        using namespace phosphor::logging;

        log<level::ERR>("Firmware Handler has invalid configuration");
        return nullptr;
    }

    return handler;
}
