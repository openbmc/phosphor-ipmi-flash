#include "lpc_aspeed.hpp"

#include "lpc_interface.hpp"

#include <cstdint>
#include <memory>
#include <utility>

namespace blobs
{

std::unique_ptr<LpcMapperInterface> LpcMapperAspeed::createAspeedMapper()
{
    /* NOTE: considered using a joint factory to create one or the other, for
     * now, separate factories.
     */
    return std::make_unique<LpcMapperAspeed>();
}

std::pair<std::uint32_t, std::uint32_t>
    LpcMapperAspeed::mapWindow(std::uint32_t address, std::uint32_t length)
{
    return std::make_pair(0, 0);
}

} // namespace blobs
