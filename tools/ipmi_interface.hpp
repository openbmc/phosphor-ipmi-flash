#pragma once

#include <cstdint>
#include <vector>

class IpmiInterface
{
  public:
    virtual ~IpmiInterface() = default;

    virtual int sendPacket(const std::vector<std::uint8_t>& data) = 0;
};
