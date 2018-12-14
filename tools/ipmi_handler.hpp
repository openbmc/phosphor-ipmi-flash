#pragma once

#include "internal/sys.hpp"
#include "ipmi_interface.hpp"

namespace host_tool
{

class IpmiHandler : public IpmiInterface
{
  public:
    explicit IpmiHandler(const internal::Sys* sys = &internal::sys_impl) :
        sys(sys){};

    ~IpmiHandler() = default;
    IpmiHandler(const IpmiHandler&) = delete;
    IpmiHandler& operator=(const IpmiHandler&) = delete;
    IpmiHandler(IpmiHandler&&) = default;
    IpmiHandler& operator=(IpmiHandler&&) = default;

    /**
     * Attempt to open the device node.
     *
     * @return true on success, failure otherwise.
     */
    bool open();

    std::vector<std::uint8_t>
        sendPacket(const std::vector<std::uint8_t>& data) override;

  private:
    const internal::Sys* sys;
    /** TODO: Use a smart file descriptor when it's ready.  Until then only
     * allow moving this object.
     */
    int fd = -1;
};

} // namespace host_tool
