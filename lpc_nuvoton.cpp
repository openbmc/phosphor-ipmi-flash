#include "lpc_nuvoton.hpp"

#include "lpc_interface.hpp"

#include <cstdint>
#include <memory>
#include <utility>

namespace blobs
{

std::unique_ptr<LpcMapperInterface> LpcMapperNuvoton::createNuvotonMapper()
{
    /* NOTE: Considered making one factory for both types. */
    return std::make_unique<LpcMapperNuvoton>();
}

std::pair<std::uint32_t, std::uint32_t>
    LpcMapperNuvoton::mapWindow(std::uint32_t address, std::uint32_t length)
{
    return std::make_pair(0, 0);
}

} // namespace blobs
