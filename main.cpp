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
namespace
{
HashFileHandler hashHandler;
StaticLayoutHandler staticLayoutHandler(STATIC_HANDLER_STAGED_NAME);

#ifdef ENABLE_LPC_BRIDGE
#ifdef ASPEED_LPC
LpcDataHandler lpcDataHandler(LpcMapperAspeed::createAspeedMapper());
#elif NUVOTON_LPC
LpcDataHandler lpcDataHandler(LpcMapperNuvoton::createNuvotonMapper());
#else
#error "You must specify a hardware implementation."
#endif
#endif

PciDataHandler pciDataHandler(PCI_PHYSICAL_ADDRESS);

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

#ifdef __cplusplus
extern "C" {
#endif

std::unique_ptr<blobs::GenericBlobInterface> createHandler();

#ifdef __cplusplus
}
#endif

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
