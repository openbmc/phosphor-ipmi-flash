#include "firmware_handler.hpp"

#include <blobs-ipmid/manager.hpp>
#include <memory>
#include <phosphor-logging/log.hpp>

namespace blobs
{
using namespace phosphor::logging;

void setupFirmwareHandler() __attribute__((constructor));

void setupFirmwareHandler()
{
    auto* manager = getBlobManager();
    if (!manager->registerHandler(
            FirmwareBlobHandler::CreateFirmwareBlobHandler()))
    {
        log<level::ERR>("Failed to register Firmware Handler");
    }
}

} // namespace blobs
