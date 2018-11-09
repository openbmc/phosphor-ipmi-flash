#include "pci_handler.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

namespace blobs
{

bool PciDataHandler::open()
{
    /* TODO: For the ASPEED P2A driver, this method will enable the memory
     * region to use.
     */
    return false;
}

bool PciDataHandler::close()
{
    /* TODO: Turn off the P2A bridge and region to disable host-side access.
     */
    return false;
}

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
    std::memcpy(bytes.data(), &reply, sizeof(reply));

    return bytes;
}

} // namespace blobs
