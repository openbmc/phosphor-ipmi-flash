#pragma once

/* NOTE: IIRC, wak@ is working on exposing some of this in stdplus, so we can
 * transition when that's ready.
 *
 * Copied some from gpioplus to enable unit-testing of lpc nuvoton and later
 * other pieces.
 */

#include <sys/mman.h>

#include <cinttypes>
#include <cstddef>

namespace blobs
{
namespace flash
{
namespace internal
{

/**
 * @class Sys
 * @brief Overridable direct syscall interface
 */
class Sys
{
  public:
    virtual ~Sys() = default;

    virtual int open(const char* pathname, int flags) const = 0;
    virtual int close(int fd) const = 0;
    virtual void* mmap(void* addr, size_t length, int prot, int flags, int fd,
                       off_t offset) const = 0;
    virtual int munmap(void* addr, size_t length) const = 0;
    virtual int getpagesize() const = 0;
};

/**
 * @class SysImpl
 * @brief syscall concrete implementation
 * @details Passes through all calls to the normal linux syscalls
 */
class SysImpl : public Sys
{
  public:
    int open(const char* pathname, int flags) const override;
    int close(int fd) const override;
    void* mmap(void* addr, size_t length, int prot, int flags, int fd,
               off_t offset) const override;
    int munmap(void* addr, size_t length) const override;
    int getpagesize() const override;
};

/** @brief Default instantiation of sys */
extern SysImpl sys_impl;

} // namespace internal
} // namespace flash
} // namespace blobs
