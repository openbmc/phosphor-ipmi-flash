#pragma once

#include <cstdint>
#include <utility>

namespace blobs
{

/**
 * Different LPC memory map implementations may require different mechanisms for
 * specific tasks such as mapping the memory window.
 */
class LpcMapperInterface
{
  public:
    virtual ~LpcMapperInterface() = default;

    /**
     * Returns a windowOffset and windowSize if the requested window was mapped.
     *
     * TODO: If the length requested is too large, windowSize will be written with the max size that the
     * BMC can map and returns false.
     *
     * @param[in] address - The address for mapping (passed to LPC window)
     * @param[in] length - The length of the region
     * @return windowOffset, windowSize - The offset into the window and
     * length of the region.  On failure, length is set to 0.
     */
    virtual std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) = 0;
};

} // namespace blobs
