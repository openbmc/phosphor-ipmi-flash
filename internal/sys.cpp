#include "sys.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace blobs
{
namespace flash
{
namespace internal
{

int SysImpl::open(const char* pathname, int flags) const
{
    return ::open(pathname, flags);
}

int SysImpl::close(int fd) const
{
    return ::close(fd);
}

void* SysImpl::mmap(void* addr, size_t length, int prot, int flags, int fd,
                    off_t offset) const
{
    return ::mmap(addr, length, prot, flags, fd, offset);
}

int SysImpl::munmap(void* addr, size_t length) const
{
    return ::munmap(addr, length);
}

int SysImpl::getpagesize() const
{
    return ::getpagesize();
}

SysImpl sys_impl;

} // namespace internal
} // namespace flash
} // namespace blobs
