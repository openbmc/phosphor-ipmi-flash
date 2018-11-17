#pragma once

#include "lpc_interface.hpp"

#include <memory>

namespace blobs
{

class LpcMapperAspeed : public LpcMapperInterface
{
  public:
    static std::unique_ptr<LpcMapperInterface> createAspeedMapper();

    /* TODO: Needs reserved memory region's physical address and size. */
    LpcMapperAspeed() = default;

    std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) override;
};

} // namespace blobs
