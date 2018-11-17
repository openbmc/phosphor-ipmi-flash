#include "lpc_nuvoton.hpp"

#include "lpc_interface.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <utility>

namespace blobs
{
using std::uint16_t;
using std::uint32_t;
using std::uint8_t;

std::unique_ptr<LpcMapperInterface> LpcMapperNuvoton::createNuvotonMapper()
{
    /* NOTE: Considered making one factory for both types. */
    return std::make_unique<LpcMapperNuvoton>();
}

/*
 * The host buffer address is configured by host through
 * SuperIO. On BMC side the max memory can be mapped is 4kB with the caveat that
 * first byte of the buffer is reserved as host/BMC semaphore and not usable as
 * shared memory.
 *
 * Mapper returns success for (addr, len) where (addr & 0x7) == 4 and len <=
 * (4096 - 4). Otherwise, mapper returns either
 *   - WindowOffset = 4 and WindowSize = len - 4 if (addr & 0x7) == 0
 *   - WindowSize = 0 means that the region cannot be mapped otherwise
 */
std::pair<std::uint32_t, std::uint32_t>
    LpcMapperNuvoton::mapWindow(std::uint32_t address, std::uint32_t length)
{
    /* We reserve the first 4 bytes from the mapped region; the first byte
     * is shared semaphore, and the number of 4 is for alignment.
     */
    const uint32_t bmcMapReserveBytes = 4;
    const uint32_t bmcMapMaxSizeBytes = 4 * 1024 - bmcMapReserveBytes;

    if (length <= bmcMapReserveBytes)
    {
        std::fprintf(stderr, "window size %" PRIx32 " too small to map.\n",
                     length);
        return std::make_pair(0, 0);
    }

    if (length > bmcMapMaxSizeBytes)
    {
        std::fprintf(stderr,
                     "window size %" PRIx32 " not supported. Max size 4k.\n",
                     length);
        length = bmcMapMaxSizeBytes;
    }

    /* If host requested region starts at an aligned address, return offset of 4
     * bytes so as to skip the semaphore register.
     */
    uint32_t windowOffset = bmcMapReserveBytes;
    uint32_t windowSize = length;

    const uint32_t addressOffset = address & 0x7;

    if (addressOffset == 0)
    {
        std::fprintf(stderr, "Map address offset should be 4 for Nuvoton.\n");
        return std::make_pair(0, 0);
    }
    else if (addressOffset != bmcMapReserveBytes)
    {
        std::fprintf(stderr, "Map address offset should be 4 for Nuvoton.\n");
        return std::make_pair(0, 0);
    }

    /* TODO: need a kernel driver to handle mapping configuration.
     * Until then program the register through /dev/mem.
     */
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        std::fprintf(stderr, "Failed to open /dev/mem\n");
        close(fd);
        return std::make_pair(0, 0);
    }

    const uint32_t bmcMapConfigBaseAddr = 0xc0001000;
    const uint32_t bmcMapConfigWindowSizeOffset = 0x7;
    const uint32_t bmcMapConfigWindowBaseOffset = 0xa;
    const uint8_t bmcWindowSizeValue = 0xc;     // 4k
    const uint16_t bmcWindowBaseValue = 0x8000; // BMC phyAddr from 0xc0008000

    auto mapBasePtr = reinterpret_cast<uint8_t*>(
        mmap(nullptr, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd,
             bmcMapConfigBaseAddr));

    uint8_t* bmcWindowSize = mapBasePtr + bmcMapConfigWindowSizeOffset;
    uint16_t* bmcWindowBase =
        reinterpret_cast<uint16_t*>(mapBasePtr + bmcMapConfigWindowBaseOffset);

    *bmcWindowSize = bmcWindowSizeValue;
    *bmcWindowBase = bmcWindowBaseValue;

    munmap(mapBasePtr, getpagesize());
    close(fd);

    return std::make_pair(windowOffset, windowSize);
}

} // namespace blobs
