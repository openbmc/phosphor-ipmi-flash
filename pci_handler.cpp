#include "pci_handler.hpp"

#include <cstdint>
#include <vector>

namespace blobs
{

std::vector<std::uint8_t> PciDataHandler::copyFrom(std::uint32_t length)
{
    /* TODO: implement this. */
    return {};
}

bool PciDataHandler::write(const std::vector<std::uint8_t>& configuration)
{
    /* PCI handler doesn't require configuration write, only read. */
    return false;
}

std::vector<std::uint8_t> PciDataHandler::read()
{
    /* PCI handler does require returning a configuration from read. */
    struct PciConfigResponse reply;
    reply.address = 0;

    std::vector<std::uint8_t> bytes;
    bytes.resize(sizeof(reply));

    return bytes;
}

} // namespace blobs
