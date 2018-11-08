#include "lpc_handler.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

namespace blobs
{

std::vector<std::uint8_t> LpcDataHandler::copyFrom(std::uint32_t length)
{
    /* TODO: implement this. */
    return {};
}

bool LpcDataHandler::write(const std::vector<std::uint8_t>& configuration)
{
    struct LpcRegion lpcRequest;

    if (configuration.size() != sizeof(lpcRequest))
    {
        return false;
    }

    std::memcpy(&lpcRequest, configuration.data(), configuration.size());
    /* TODO: Implement the call to the driver or aspeed lpc ctrl library to send
     * ioctl.
     */

    return false;
}

std::vector<std::uint8_t> LpcDataHandler::read()
{
    return {};
}

} // namespace blobs
