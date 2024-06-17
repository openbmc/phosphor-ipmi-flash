#pragma once

#include <cstdlib>

namespace host_tool
{

class HostIoInterface
{
  public:
    virtual ~HostIoInterface() = default;

    /**
     * Attempt to read bytes from offset to the destination from the host
     * memory device.
     *
     * @param[in] offset - offset into the host memory device.
     * @param[in] length - the number of bytes to copy from source.
     * @param[in] destination - where to write the bytes.
     * @return true on success, false on failure (such as unable to initialize
     * device).
     */
    virtual bool read(const std::size_t offset, const std::size_t length,
                      void* const destination) = 0;

    /**
     * Attempt to write bytes from source to offset into the host memory device.
     *
     * @param[in] offset - offset into the host memory device.
     * @param[in] length - the number of bytes to copy from source.
     * @param[in] source - the source of the bytes to copy to the memory device.
     * @return true on success, false on failure (such as unable to initialize
     * device).
     */
    virtual bool write(const std::size_t offset, const std::size_t length,
                       const void* const source) = 0;
};

} // namespace host_tool
