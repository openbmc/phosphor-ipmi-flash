#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace blobs
{

/**
 * Different LPC (or P2a) memory map implementations may require different
 * mechanisms for specific tasks such as mapping the memory window or copying
 * out data.
 */
class HardwareMapperInterface
{
  public:
    virtual ~HardwareMapperInterface() = default;

    /**
     * Close the mapper.  This could mean, send an ioctl to turn off the region,
     * or unmap anything mmapped.
     */
    virtual void close() = 0;

    /**
     * Returns a windowOffset and windowSize if the requested window was mapped.
     *
     * TODO: If the length requested is too large, windowSize will be written
     * with the max size that the BMC can map and returns false.
     *
     * @param[in] address - The address for mapping (passed to LPC window)
     * @param[in] length - The length of the region
     * @return windowOffset, windowSize - The offset into the window and
     * length of the region.  On failure, length is set to 0.
     */
    virtual std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) = 0;

    /**
     * Returns the bytes from the mapped window.
     *
     * @param[in] length - the number of bytes to copy.
     * @return the bytes copied out of the region.
     */
    virtual std::vector<std::uint8_t> copyFrom(std::uint32_t length) = 0;
};

} // namespace blobs
