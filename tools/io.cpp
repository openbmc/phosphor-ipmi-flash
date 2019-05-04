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

    // addr, length, prot, flags, fd, offset
    devMemMapped =
        sys->mmap(0, length, PROT_WRITE, MAP_SHARED, devMemFd, offset);
    if (devMemMapped == MAP_FAILED)
    {
        return false; /* but leave the file open. */
    }

    /* Copy the bytes. */
    std::memcpy(destination, devMemMapped, length);

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

    // addr, length, prot, flags, fd, offset
    devMemMapped =
        sys->mmap(0, length, PROT_WRITE, MAP_SHARED, devMemFd, offset);
    if (devMemMapped == MAP_FAILED)
    {
        return false; /* but leave the file open. */
    }

    /* Copy the bytes. */
    std::memcpy(devMemMapped, source, length);

    /* Close the map between writes for now. */
    sys->munmap(devMemMapped, length);

    return true;
}

} // namespace host_tool
