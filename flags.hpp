#pragma once

#include <cstdint>

namespace ipmi_flash
{

class FirmwareFlags
{
  public:
    enum UpdateFlags : std::uint16_t
    {
        openRead = (1 << 0),  /* Flag for reading. */
        openWrite = (1 << 1), /* Flag for writing. */
        ipmi = (1 << 8), /* Expect to send contents over IPMI BlockTransfer. */
        p2a = (1 << 9),  /* Expect to send contents over P2A bridge. */
        lpc = (1 << 10), /* Expect to send contents over LPC bridge. */
        net = (1 << 11), /* Expect to send contents over network bridge. */
    };
};

} // namespace ipmi_flash
