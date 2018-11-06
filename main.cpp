#include "config.h"

#include "firmware_handler.hpp"

#include <blobs-ipmid/manager.hpp>
#include <cstdint>
#include <memory>
#include <phosphor-logging/log.hpp>

namespace blobs
{
using namespace phosphor::logging;

std::vector<std::string> supportedFirmware = {
    "/flash/hash",
#ifdef ENABLE_STATIC_LAYOUT
    "/flash/image",
#endif
};

std::uint32_t supportedTransports = FirmwareUpdateFlags::bt;

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
#ifdef ENABLE_PCI_BRIDGE
    supportedTransports |= FirmwareUpdateFlags::p2a;
#endif
#ifdef ENABLE_LPC_BRIDGE
    supportedTransports |= FirmwareUpdateFlags::lpc;
#endif

    auto* manager = getBlobManager();
    if (!manager->registerHandler(
            FirmwareBlobHandler::CreateFirmwareBlobHandler(
                supportedFirmware, supportedTransports)))
    {
        log<level::ERR>("Failed to register Firmware Handler");
    }
}

} // namespace blobs
