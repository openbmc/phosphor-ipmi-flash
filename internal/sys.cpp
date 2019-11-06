/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sys.hpp"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>

namespace internal
{

std::int64_t SysImpl::getSize(const char* pathname) const
{
    struct stat results;
    int rc = ::stat(pathname, &results);
    if (rc)
    {
        return 0;
    }

    return static_cast<std::int64_t>(results.st_size);
}

int SysImpl::open(const char* pathname, int flags) const
{
    return ::open(pathname, flags);
}

int SysImpl::read(int fd, void* buf, std::size_t count) const
{
    return static_cast<int>(::read(fd, buf, count));
}

int SysImpl::pread(int fd, void* buf, std::size_t count, off_t offset) const
{
    return static_cast<int>(::pread(fd, buf, count, offset));
}

int SysImpl::pwrite(int fd, const void* buf, std::size_t count,
                    off_t offset) const
{
    return static_cast<int>(::pwrite(fd, buf, count, offset));
}

int SysImpl::close(int fd) const
{
    return ::close(fd);
}

void* SysImpl::mmap(void* addr, std::size_t length, int prot, int flags, int fd,
                    off_t offset) const
{
    return ::mmap(addr, length, prot, flags, fd, offset);
}

int SysImpl::munmap(void* addr, std::size_t length) const
{
    return ::munmap(addr, length);
}

int SysImpl::getpagesize() const
{
    return ::getpagesize();
}

int SysImpl::ioctl(int fd, unsigned long request, void* param) const
{
    return ::ioctl(fd, request, param);
}

int SysImpl::poll(struct pollfd* fds, nfds_t nfds, int timeout) const
{
    return ::poll(fds, nfds, timeout);
}

SysImpl sys_impl;

} // namespace internal
