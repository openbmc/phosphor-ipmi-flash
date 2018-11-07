#include "config.h"

#include "firmware_handler.hpp"
#include "image_handler.hpp"
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
StaticLayoutHandler staticLayoutHandler;

std::vector<HandlerPack> supportedFirmware = {
#ifdef ENABLE_STATIC_LAYOUT
    {"/flash/image", &staticLayoutHandler},
#endif
};

std::uint16_t supportedTransports =
    FirmwareBlobHandler::FirmwareUpdateFlags::bt;
} // namespace

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
#ifdef ENABLE_PCI_BRIDGE
    supportedTransports |= FirmwareBlobHandler::FirmwareUpdateFlags::p2a;
#endif
#ifdef ENABLE_LPC_BRIDGE
    supportedTransports |= FirmwareBlobHandler::FirmwareUpdateFlags::lpc;
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
