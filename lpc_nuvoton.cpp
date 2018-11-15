#include "lpc_nuvoton.hpp"

#include "lpc_interface.hpp"

#include <cstdint>
#include <memory>

namespace blobs
{

std::unique_ptr<LpcMapperInterface> LpcMapperNuvoton::createNuvotonMapper()
{
    /* NOTE: Considered making one factory for both types. */
    return std::make_unique<LpcMapperNuvoton>();
}

bool LpcMapperNuvoton::mapWindow(std::uint32_t* windowOffset,
                                 std::uint32_t* windowSize,
                                 std::uint32_t address, std::uint32_t length)
{
    return false;
}

} // namespace blobs
