#pragma once

#include <cstdint>

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
     * Returns true if requested window was mapped. If the length requested
     * is too large, windowSize will be written with the max size that the
     * BMC can map and returns false.
     *
     * @param[out] windowOffset - The offset into the window
     * @param[out] windowSize - The length of the region configured
     * @param[in] address - The address for mapping (passed to LPC window)
     * @param[in] length - The length of the region
     */
    virtual bool mapWindow(std::uint32_t* windowOffset,
                           std::uint32_t* windowSize, std::uint32_t address,
                           std::uint32_t length) = 0;
};

} // namespace blobs
