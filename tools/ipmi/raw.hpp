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
    virtual struct IpmiResponse RawWithTries(const std::vector<uint8_t>& buffer,
                                             int tries) = 0;
};

class IpmiRaw : public RawInterface
{
  public:
    IpmiRaw() = default;

    struct IpmiResponse Raw(const std::vector<uint8_t>& buffer) override;
    struct IpmiResponse RawWithTries(const std::vector<uint8_t>& buffer,
                                     int tries) override;
};
