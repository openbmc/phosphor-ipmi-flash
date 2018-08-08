#pragma once

#include "raw.hpp"

class MockRaw : public RawInterface
{
  public:
    MOCK_METHOD1(Raw, struct IpmiResponse(const std::vector<uint8_t>&));
    MOCK_METHOD2(RawWithTries,
                 struct IpmiResponse(const std::vector<uint8_t>&, int));
};
