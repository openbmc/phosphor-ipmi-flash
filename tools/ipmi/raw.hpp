#pragma once

#include <vector>

extern "C" {
#include "ipmitoolintf.h"
} // extern "C"

class RawInterface
{
  public:
    virtual ~RawInterface() = default;

    virtual struct IpmiResponse Raw(const std::vector<uint8_t>& buffer) = 0;
};

class IpmiRaw : public RawInterface
{
  public:
    IpmiRaw() = default;

    struct IpmiResponse Raw(const std::vector<uint8_t>& buffer) override;
};
