#pragma once

#include "lpc_interface.hpp"

#include <gmock/gmock.h>

namespace blobs
{

class LpcInterfaceMock : public LpcMapperInterface
{
  public:
    virtual ~LpcInterfaceMock() = default;

    MOCK_METHOD2(mapWindow,
                 std::pair<std::uint32_t, std::uint32_t>(std::uint32_t,
                                                         std::uint32_t));
};

} // namespace blobs
