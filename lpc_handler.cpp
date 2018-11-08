#include "lpc_handler.hpp"

#include <cstdint>
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
    /* TODO: LPC needs this, so please implement. */
    return false;
}

std::vector<std::uint8_t> LpcDataHandler::read()
{
    return {};
}

} // namespace blobs
