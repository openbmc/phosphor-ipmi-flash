#include "config.h"

#include "firmware_handler.hpp"

#include <blobs-ipmid/manager.hpp>
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

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
    auto* manager = getBlobManager();
    if (!manager->registerHandler(
            FirmwareBlobHandler::CreateFirmwareBlobHandler(supportedFirmware)))
    {
        log<level::ERR>("Failed to register Firmware Handler");
    }
}

} // namespace blobs
