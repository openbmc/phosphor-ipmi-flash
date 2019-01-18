#pragma once

#include "internal/sys.hpp"

#include <cstdint>

namespace host_tool
{

class HostIoInterface
{
  public:
    virtual ~HostIoInterface() = default;

    /**
     * Attempt to write bytes from source to offset into the host memory device.
     *
     * @param[in] offset - offset into the host memory device.
     * @param[in] length - the number of bytes to copy from source.
     * @param[in] source - the souce of the bytes to copy to the memory device.
     * @return true on success, false on failure (such as unable to initialize
     * device).
     */
    virtual bool write(const std::size_t offset, const std::size_t length,
                       const void* const source) = 0;
};

class DevMemDevice : public HostIoInterface
{
  public:
    explicit DevMemDevice(const internal::Sys* sys = &internal::sys_impl) :
        sys(sys)
    {
    }

    bool write(const std::size_t offset, const std::size_t length,
               const void* const source) override;

  private:
    const internal::Sys* sys;
};

} // namespace host_tool
