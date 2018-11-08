#pragma once

#include "data_handler.hpp"

#include <cstdint>
#include <vector>

namespace blobs
{

/** P2A configuration response. */
struct PciConfigResponse
{
    std::uint32_t address;
} __attribute__((packed));

/**
 * Data handler for reading and writing data via the P2A bridge.
 */
class PciDataHandler : public DataInterface
{
  public:
    PciDataHandler() = default;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
    bool write(const std::vector<std::uint8_t>& configuration) override;
    std::vector<std::uint8_t> read() override;
};

} // namespace blobs
