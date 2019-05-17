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
#include "image_handler.hpp"
#include "lpc_aspeed.hpp"
#include "lpc_handler.hpp"
#include "lpc_nuvoton.hpp"
#include "pci_handler.hpp"
#include "util.hpp"
#include "verify.hpp"

#include <cstdint>
#include <memory>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>

namespace blobs
{
namespace
{
FileHandler hashHandler(HASH_FILENAME);
FileHandler staticLayoutHandler(STATIC_HANDLER_STAGED_NAME);
FileHandler ubitarballHandler(TARBALL_STAGED_NAME);

/* The maximum external buffer size we expect is 64KB. */
static constexpr std::size_t memoryRegionSize = 64 * 1024UL;

#ifdef ENABLE_LPC_BRIDGE
#if defined(ASPEED_LPC)
LpcDataHandler lpcDataHandler(
    LpcMapperAspeed::createAspeedMapper(MAPPED_ADDRESS, memoryRegionSize));
#elif defined(NUVOTON_LPC)
LpcDataHandler
    lpcDataHandler(LpcMapperNuvoton::createNuvotonMapper(MAPPED_ADDRESS));
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

std::vector<HandlerPack> supportedFirmware = {
    {hashBlobId, &hashHandler},
#ifdef ENABLE_STATIC_LAYOUT
    {staticLayoutBlobId, &staticLayoutHandler},
#endif
#ifdef ENABLE_TARBALL_UBI
    {ubiTarballBlobId, &ubitarballHandler},
#endif
};

std::vector<DataHandlerPack> supportedTransports = {
    {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
#ifdef ENABLE_PCI_BRIDGE
    {FirmwareBlobHandler::UpdateFlags::p2a, &pciDataHandler},
#endif
#ifdef ENABLE_LPC_BRIDGE
    {FirmwareBlobHandler::UpdateFlags::lpc, &lpcDataHandler},
#endif
};

} // namespace

} // namespace blobs

extern "C" {
std::unique_ptr<blobs::GenericBlobInterface> createHandler();
}

std::unique_ptr<blobs::GenericBlobInterface> createHandler()
{
    using namespace phosphor::logging;

    auto handler = blobs::FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs::supportedFirmware,
        blobs::supportedTransports, CreateDefaultVerification(sdbusplus::bus::new_default(), VERIFY_STATUS_FILENAME, VERIFY_DBUS_SERVICE));

    if (!handler)
    {
        log<level::ERR>("Firmware Handler has invalid configuration");
        return nullptr;
    }

    return std::move(handler);
}
