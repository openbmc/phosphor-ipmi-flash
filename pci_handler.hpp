#pragma once

#include "data_handler.hpp"
#include "internal/sys.hpp"

#include <cstdint>
#include <string>
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
 *
 * @note: Currently implemented to support only aspeed-p2a-ctrl.
 */
class PciDataHandler : public DataInterface
{
  public:
    PciDataHandler(std::uint32_t regionAddress, const internal::Sys* sys = &internal::sys_impl) :
        regionAddress(regionAddress), sys(sys){};

    bool open() override;
    bool close() override;
    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
    bool writeMeta(const std::vector<std::uint8_t>& configuration) override;
    std::vector<std::uint8_t> readMeta() override;

  private:
    std::uint32_t regionAddress;
    const internal::Sys* sys;
    static const std::string p2aControlPath;
};

} // namespace blobs
