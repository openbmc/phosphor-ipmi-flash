#pragma once

#include <cstdint>
#include <vector>

class IpmiInterface
{
  public:
    virtual ~IpmiInterface() = default;

    /**
     * Send an IPMI packet to the BMC.
     *
     * @param[in] data - a vector of the IPMI packet contents.
     * @return non-zero on failure.
     */
    virtual int sendPacket(const std::vector<std::uint8_t>& data) = 0;
};
