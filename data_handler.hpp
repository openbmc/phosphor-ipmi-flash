#pragma once

#include <cstdint>
#include <vector>

namespace blobs
{

/**
 * Each data transport mechanism must implement the DataInterface.
 */
class DataInterface
{
  public:
    virtual ~DataInterface() = default;

    /**
     * Copy bytes from external interface (blocking call).
     *
     * @param[in] length - number of bytes to copy
     * @return the bytes read
     */
    virtual std::vector<std::uint8_t> copyFrom(std::uint32_t length) = 0;
};

struct DataHandlerPack
{
    std::uint16_t bitmask;
    DataInterface* handler;
};

} // namespace blobs
