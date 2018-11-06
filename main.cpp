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
#ifdef ENABLE_STATIC_LAYOUT
    "/flash/image",
#endif
};

std::uint16_t supportedTransports =
    static_cast<std::uint16_t>(FirmwareUpdateFlags::bt);

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
#ifdef ENABLE_PCI_BRIDGE
    supportedTransports |= static_cast<std::uint16_t>(FirmwareUpdateFlags::p2a);
#endif
#ifdef ENABLE_LPC_BRIDGE
    supportedTransports |= static_cast<std::uint16_t>(FirmwareUpdateFlags::lpc);
#endif

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
