#pragma once

#include "internal/sys.hpp"

#include <unistd.h>

#include <gmock/gmock.h>

namespace internal
{

class InternalSysMock : public Sys
{
  public:
    virtual ~InternalSysMock() = default;

    MOCK_CONST_METHOD2(open, int(const char*, int));
    MOCK_CONST_METHOD1(close, int(int));
    MOCK_CONST_METHOD6(mmap, void*(void*, std::size_t, int, int, int, off_t));
    MOCK_CONST_METHOD2(munmap, int(void*, std::size_t));
    MOCK_CONST_METHOD0(getpagesize, int());
    MOCK_CONST_METHOD3(ioctl, int(int, unsigned long, void*));
};

} // namespace internal
