#pragma once

#include <cstdint>

namespace ipmi_flash
{

struct ExtChunkHdr
{
    std::uint32_t length; /* Length of the data queued (little endian). */
} __attribute__((packed));

} // namespace ipmi_flash
