#pragma once

#include <cstdint>
#include <vector>

#include <gmock/gmock.h>

class CrcInterface
{
  public:
    virtual ~CrcInterface() = default;

    virtual std::uint16_t generateCrc(
        const std::vector<std::uint8_t>& data) const = 0;
};

class CrcMock : public CrcInterface
{
  public:
    virtual ~CrcMock() = default;
    MOCK_METHOD(std::uint16_t, generateCrc, (const std::vector<std::uint8_t>&),
                (const, override));
};
