#include "lpc_aspeed.hpp"

#include "lpc_interface.hpp"

#include <cstdint>
#include <memory>

namespace blobs
{

std::unique_ptr<LpcMapperInterface> LpcMapperAspeed::createAspeedMapper()
{
    /* NOTE: considered using a joint factory to create one or the other, for
     * now, separate factories.
     */
    return std::make_unique<LpcMapperAspeed>();
}

bool LpcMapperAspeed::mapWindow(std::uint32_t* windowOffset,
                                std::uint32_t* windowSize,
                                std::uint32_t address, std::uint32_t length)
{
    return false;
}

} // namespace blobs
