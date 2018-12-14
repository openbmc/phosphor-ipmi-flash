#pragma once

#include <cstdint>
#include <vector>

namespace host_tool
{

class IpmiInterface
{
  public:
    virtual ~IpmiInterface() = default;

    /**
     * Send an IPMI packet to the BMC.
     *
     * @param[in] data - a vector of the IPMI packet contents.
     * @return the bytes returned.
     * @throws IpmiException on failure.
     */
    virtual std::vector<std::uint8_t>
        sendPacket(const std::vector<std::uint8_t>& data) = 0;
};

} // namespace host_tool
