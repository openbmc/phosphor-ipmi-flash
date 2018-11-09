#pragma once

#include "data_handler.hpp"

#include <cstdint>
#include <vector>

namespace blobs
{

struct LpcRegion
{
    std::uint32_t address; /* Host LPC address where the chunk is to be mapped. */
    std::uint32_t length; /* Size of the chunk to be mapped. */
} __attribute__((packed));

class LpcDataHandler : public DataInterface
{

  public:
    LpcDataHandler() = default;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
    bool write(const std::vector<std::uint8_t>& configuration) override;
    std::vector<std::uint8_t> read() override;
};

} // namespace blobs
