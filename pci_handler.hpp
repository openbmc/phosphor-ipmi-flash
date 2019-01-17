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
    explicit PciDataHandler(std::uint32_t regionAddress) :
        regionAddress(regionAddress){};

    bool open() override;
    bool close() override;
    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
    bool writeMeta(const std::vector<std::uint8_t>& configuration) override;
    std::vector<std::uint8_t> readMeta() override;

  private:
    std::uint32_t regionAddress;
};

} // namespace blobs
