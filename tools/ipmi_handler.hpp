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

    std::vector<std::uint8_t>
        sendPacket(const std::vector<std::uint8_t>& data) override;

  private:
    const internal::Sys* sys;
};

} // namespace host_tool
