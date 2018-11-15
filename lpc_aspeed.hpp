#pragma once

#include "lpc_interface.hpp"

namespace blobs
{

class LpcMapperAspeed : public LpcMapperInterface
{
  public:
    static std::unique_ptr<LpcMapperInterface> createAspeedMapper();

    /* TODO: Needs reserved memory region's physical address and size. */
    LpcMapperAspeed() = default;

    bool mapWindow(std::uint32_t* windowOffset, std::uint32_t* windowSize,
                   std::uint32_t address, std::uint32_t length) override;
};

} // namespace blobs
