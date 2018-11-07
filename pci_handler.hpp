#pragma once

#include "data_handler.hpp"

#include <cstdint>
#include <vector>

namespace blobs
{

class PciDataHandler : public DataInterface
{

  public:
    PciDataHandler() = default;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
};

} // namespace blobs
