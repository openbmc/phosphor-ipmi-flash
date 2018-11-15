#include "config.h"

#include "file_handler.hpp"
#include "firmware_handler.hpp"
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
namespace
{
FileHandler hashHandler(HASH_FILENAME);
StaticLayoutHandler staticLayoutHandler(STATIC_HANDLER_STAGED_NAME);

#ifdef ENABLE_LPC_BRIDGE
#if defined(ASPEED_LPC)
LpcDataHandler lpcDataHandler(MAPPED_ADDRESS,
                              LpcMapperAspeed::createAspeedMapper());
#elif defined(NUVOTON_LPC)
LpcDataHandler lpcDataHandler(MAPPED_ADDRESS,
                              LpcMapperNuvoton::createNuvotonMapper());
#else
#error "You must specify a hardware implementation."
#endif
#endif

PciDataHandler pciDataHandler(MAPPED_ADDRESS);

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

} // namespace blobs

extern "C" {
std::unique_ptr<blobs::GenericBlobInterface> createHandler();
}

std::unique_ptr<blobs::GenericBlobInterface> createHandler()
{
    using namespace phosphor::logging;

    auto handler = blobs::FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs::supportedFirmware, blobs::supportedTransports);

    if (!handler)
    {
        log<level::ERR>("Firmware Handler has invalid configuration");
        return nullptr;
    }

    return std::move(handler);
}
