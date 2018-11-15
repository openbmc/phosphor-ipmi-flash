#include "lpc_handler.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

namespace blobs
{

bool LpcDataHandler::open()
{
    /* For the ASPEED LPC CTRL driver, the ioctl is required to set up the
     * window, with information from write() below.
     */
    return true;
}

bool LpcDataHandler::close()
{
    /* TODO: implement ioctl call to close window. */
    return false;
}

std::vector<std::uint8_t> LpcDataHandler::copyFrom(std::uint32_t length)
{
    /* TODO: implement this */
    return {};
}

bool LpcDataHandler::write(const std::vector<std::uint8_t>& configuration)
{
    struct LpcRegion lpcRegion;

    if (configuration.size() != sizeof(lpcRegion))
    {
        return false;
    }

    std::memcpy(&lpcRegion, configuration.data(), configuration.size());

    std::uint32_t windowOffset;
    std::uint32_t windowSize;

    /* TODO: LpcRegion sanity checking. */

    std::tie(windowOffset, windowSize) =
        mapper->mapWindow(lpcRegion.address, lpcRegion.length);
    if (windowSize == 0)
    {
        /* Failed to map region. */
    }

    return false;
}

std::vector<std::uint8_t> LpcDataHandler::read()
{
    return {};
}

} // namespace blobs
