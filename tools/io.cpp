#include "io.hpp"

#include "internal/sys.hpp"

#include <fcntl.h>

#include <cstdint>
#include <cstring>
#include <string>

namespace host_tool
{

const std::string DevMemDevice::devMemPath = "/dev/mem";

bool DevMemDevice::read(const std::size_t offset, const std::size_t length,
                        void* const destination)
{
    devMemFd = sys->open(devMemPath.c_str(), O_RDONLY);
    if (devMemFd < 0)
    {
        return false;
    }

    /* Map based on aligned addresses - behind the scenes. */
    const std::size_t alignedDiff = offset % sys->getpagesize();
    const std::size_t alignedOffset = offset - alignedDiff;
    const std::size_t alignedSize = length + alignedDiff;

    // addr, length, prot, flags, fd, offset
    devMemMapped = sys->mmap(0, alignedSize, PROT_READ, MAP_SHARED, devMemFd,
                             alignedOffset);
    if (devMemMapped == MAP_FAILED)
    {
        std::fprintf(stderr, "Failed to mmap at offset: 0x%lx, length: %lu\n",
                     offset, length);
        sys->close(devMemFd);
        return false;
    }

    void* alignedSource =
        static_cast<std::uint8_t*>(devMemMapped) + alignedDiff;

    /* Copy the bytes. */
    std::memcpy(destination, alignedSource, length);

    /* Close the map between reads for now. */
    sys->munmap(devMemMapped, length);
    sys->close(devMemFd);

    return true;
}

bool DevMemDevice::write(const std::size_t offset, const std::size_t length,
                         const void* const source)
{
    devMemFd = sys->open(devMemPath.c_str(), O_RDWR);
    if (devMemFd < 0)
    {
        std::fprintf(stderr, "Failed to open /dev/mem for writing\n");
        return false;
    }

    /* Map based on aligned addresses - behind the scenes. */
    const std::size_t alignedDiff = offset % sys->getpagesize();
    const std::size_t alignedOffset = offset - alignedDiff;
    const std::size_t alignedSize = length + alignedDiff;

    // addr, length, prot, flags, fd, offset
    devMemMapped = sys->mmap(0, alignedSize, PROT_WRITE, MAP_SHARED, devMemFd,
                             alignedOffset);

    if (devMemMapped == MAP_FAILED)
    {
        std::fprintf(stderr, "Failed to mmap at offset: 0x%lx, length: %lu\n",
                     offset, length);
        sys->close(devMemFd);
        return false;
    }

    void* alignedDestination =
        static_cast<std::uint8_t*>(devMemMapped) + alignedDiff;

    /* Copy the bytes. */
    std::memcpy(alignedDestination, source, length);

    /* Close the map between writes for now. */
    sys->munmap(devMemMapped, length);
    sys->close(devMemFd);

    return true;
}

} // namespace host_tool
