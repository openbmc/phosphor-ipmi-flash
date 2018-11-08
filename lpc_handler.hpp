#pragma once

#include "data_handler.hpp"

#include <cstdint>
#include <vector>

namespace blobs
{

struct LpcRegion
{
    /* Host LPC address where the chunk is to be mapped. */
    std::uint32_t address;

    /* Size of the chunk to be mapped. */
    std::uint32_t length;
} __attribute__((packed));

/**
 * Data Handler for configuration the ASPEED LPC memory region, reading and
 * writing data.
 */
class LpcDataHandler : public DataInterface
{
  public:
    LpcDataHandler() = default;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
    bool write(const std::vector<std::uint8_t>& configuration) override;
    std::vector<std::uint8_t> read() override;
};

} // namespace blobs
