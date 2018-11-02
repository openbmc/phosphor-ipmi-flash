#include "firmware_handler.hpp"

#include <blobs-ipmid/manager.hpp>
#include <memory>
#include <phosphor-logging/log.hpp>

namespace blobs
{

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
    auto* manager = getBlobManager();
    if (!manager->registerHandler(std::make_unique<FirmwareBlobHandler>()))
    {
        log<level::ERR>("Failed to register Firmware Handler");
    }
}

} // namespace blobs
