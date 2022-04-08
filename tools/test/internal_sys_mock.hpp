#pragma once

#include "internal/sys.hpp"

#include <unistd.h>

#include <cstdint>

#include <gmock/gmock.h>

namespace internal
{

class InternalSysMock : public Sys
{
  public:
    virtual ~InternalSysMock() = default;

    MOCK_METHOD(int, open, (const char*, int), (const override));
    MOCK_METHOD(int, read, (int, void*, std::size_t), (const override));
    MOCK_METHOD(int, pread, (int, void*, std::size_t, off_t), (const override));
    MOCK_METHOD(int, pwrite, (int, const void*, std::size_t, off_t),
                (const override));
    MOCK_METHOD(int, close, (int), (const override));
    MOCK_METHOD(void*, mmap, (void*, std::size_t, int, int, int, off_t),
                (const override));
    MOCK_METHOD(int, munmap, (void*, std::size_t), (const override));
    MOCK_METHOD(int, getpagesize, (), (const override));
    MOCK_METHOD(int, ioctl, (int, unsigned long, void*), (const override));
    MOCK_METHOD(int, poll, (struct pollfd*, nfds_t, int), (const override));
    MOCK_METHOD(int, socket, (int, int, int), (const override));
    MOCK_METHOD(int, connect, (int, const struct sockaddr*, socklen_t),
                (const override));
    MOCK_METHOD(ssize_t, send, (int, const void*, size_t, int),
                (const override));
    MOCK_METHOD(ssize_t, sendfile, (int, int, off_t*, size_t),
                (const override));
    MOCK_METHOD(int, getaddrinfo,
                (const char*, const char*, const struct addrinfo*,
                 struct addrinfo**),
                (const override));
    MOCK_METHOD(void, freeaddrinfo, (struct addrinfo*), (const override));
    MOCK_METHOD(std::int64_t, getSize, (const char*), (const override));
};

} // namespace internal
