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
    if (!opened)
    {
        devMemFd = sys->open(devMemPath.c_str(), O_RDWR);
        if (devMemFd < 0)
        {
            return false;
        }

        opened = true;
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
        return false; /* but leave the file open. */
    }

    void* alignedSource =
        static_cast<std::uint8_t*>(devMemMapped) + alignedDiff;

    /* Copy the bytes. */
    std::memcpy(destination, alignedSource, length);

    /* Close the map between reads for now. */
    sys->munmap(devMemMapped, length);

    return true;
}

bool DevMemDevice::write(const std::size_t offset, const std::size_t length,
                         const void* const source)
{
    if (!opened)
    {
        devMemFd = sys->open(devMemPath.c_str(), O_RDWR);
        if (devMemFd < 0)
        {
            return false;
        }

        opened = true;
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
        return false; /* but leave the file open. */
    }

    void* alignedDestination =
        static_cast<std::uint8_t*>(devMemMapped) + alignedDiff;

    /* Copy the bytes. */
    std::memcpy(alignedDestination, source, length);

    /* Close the map between writes for now. */
    sys->munmap(devMemMapped, length);

    return true;
}

} // namespace host_tool
