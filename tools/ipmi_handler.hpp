#pragma once

#include "ipmi_interface.hpp"

class IpmiHandler : public IpmiInterface
{
  public:
    IpmiHandler() = default;

    int sendPacket(const std::vector<std::uint8_t>& data) override;
};
