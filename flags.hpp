#pragma once

#include <cstdint>

namespace ipmi_flash
{

class FirmwareFlags
{
  public:
    enum UpdateFlags : std::uint16_t
    {
        openRead = (1 << 0),  // Flag for reading
        openWrite = (1 << 1), // Flag for writing
        ipmi = (0b001 << 8),  // Expect to send contents over IPMI BlockTransfer
        net = (0b011 << 8),   // Expect to send contents over network bridge
        p2a = (0b010 << 8),   // Expect to send contents over P2A bridge
        lpc = (0b100 << 8),   // Expect to send contents over LPC bridge
    };
};

} // namespace ipmi_flash
