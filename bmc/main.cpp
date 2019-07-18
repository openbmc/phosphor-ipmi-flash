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
#include <unordered_map>

namespace ipmi_flash
{
namespace
{

/* The maximum external buffer size we expect is 64KB. */
static constexpr std::size_t memoryRegionSize = 64 * 1024UL;

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

} // namespace
} // namespace ipmi_flash

extern "C" {
std::unique_ptr<blobs::GenericBlobInterface> createHandler();
}

std::unique_ptr<blobs::GenericBlobInterface> createHandler()
{
    ipmi_flash::ActionMap actionPacks = {};

#ifdef ENABLE_REBOOT_UPDATE
    static constexpr auto rebootTarget = "reboot.target";
    static constexpr auto rebootMode = "replace-irreversibly";

    auto updater = ipmi_flash::SystemdUpdateMechanism::CreateSystemdUpdate(
        sdbusplus::bus::new_default(), rebootTarget, rebootMode);
#else
    auto updater = ipmi_flash::SystemdUpdateMechanism::CreateSystemdUpdate(
        sdbusplus::bus::new_default(), UPDATE_DBUS_SERVICE);
#endif

    auto prepare = ipmi_flash::SystemdPreparation::CreatePreparation(
        sdbusplus::bus::new_default(), PREPARATION_DBUS_SERVICE);

    auto verifier = ipmi_flash::SystemdVerification::CreateVerification(
        sdbusplus::bus::new_default(), VERIFY_STATUS_FILENAME,
        VERIFY_DBUS_SERVICE);

    /* TODO: for bios should the name be, bios or /flash/bios?, these are
     * /flash/... and it simplifies a few other things later (open/etc)
     */
    std::string bmcName;
#ifdef ENABLE_STATIC_LAYOUT
    bmcName = ipmi_flash::staticLayoutBlobId;
#endif
#ifdef ENABLE_TARBALL_UBI
    bmcName = ipmi_flash::ubiTarballBlobId;
#endif

    auto bmcPack = std::make_unique<ipmi_flash::ActionPack>();
    bmcPack->preparation = std::move(prepare);
    bmcPack->verification = std::move(verifier);
    bmcPack->update = std::move(updater);
    actionPacks[bmcName] = std::move(bmcPack);

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

    supportedFirmware.push_back(std::move(ipmi_flash::HandlerPack(
        ipmi_flash::hashBlobId,
        std::make_unique<ipmi_flash::FileHandler>(HASH_FILENAME))));

#ifdef ENABLE_STATIC_LAYOUT
    supportedFirmware.push_back(std::move(
        ipmi_flash::HandlerPack(ipmi_flash::staticLayoutBlobId,
                                std::make_unique<ipmi_flash::FileHandler>(
                                    STATIC_HANDLER_STAGED_NAME))));
#endif
#ifdef ENABLE_TARBALL_UBI
    supportedFirmware.push_back(std::move(ipmi_flash::HandlerPack(
        ipmi_flash::ubiTarballBlobId,
        std::make_unique<ipmi_flash::FileHandler>(TARBALL_STAGED_NAME))));
#endif
#ifdef ENABLE_HOST_BIOS
    supportedFirmware.push_back(std::move(ipmi_flash::HandlerPack(
        ipmi_flash::biosBlobId,
        std::make_unique<ipmi_flash::FileHandler>(BIOS_STAGED_NAME))));
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
