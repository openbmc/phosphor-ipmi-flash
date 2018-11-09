#include "config.h"

#include "firmware_handler.hpp"
#include "hash_handler.hpp"
#include "image_handler.hpp"
#include "lpc_handler.hpp"
#include "pci_handler.hpp"
#include "static_handler.hpp"

#include <blobs-ipmid/manager.hpp>
#include <cstdint>
#include <memory>
#include <phosphor-logging/log.hpp>

namespace blobs
{
using namespace phosphor::logging;

namespace
{
HashFileHandler hashHandler;
StaticLayoutHandler staticLayoutHandler;
LpcDataHandler lpcDataHandler;
PciDataHandler pciDataHandler;

std::vector<HandlerPack> supportedFirmware = {
    {FirmwareBlobHandler::hashBlobID, &hashHandler},
#ifdef ENABLE_STATIC_LAYOUT
    {"/flash/image", &staticLayoutHandler},
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

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        supportedFirmware, supportedTransports);

    if (!handler)
    {
        log<level::ERR>("Firmware Handler has invalid configuration");
        return;
    }

    auto* manager = getBlobManager();

    if (!manager->registerHandler(std::move(handler)))
    {
        log<level::ERR>("Failed to register Firmware Handler");
    }
}

} // namespace blobs
