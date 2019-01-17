#pragma once

#include "data_handler.hpp"
#include "window_hw_interface.hpp"

#include <cstdint>
#include <memory>
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
    /**
     * Create an LpcDataHandler.
     *
     * @param[in] mapper - pointer to a mapper implementation to use.
     */
    LpcDataHandler(std::unique_ptr<HardwareMapperInterface> mapper) :
        mapper(std::move(mapper)), initialized(false)
    {
    }

    bool open() override;
    bool close() override;
    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;
    bool writeMeta(const std::vector<std::uint8_t>& configuration) override;
    std::vector<std::uint8_t> readMeta() override;

  private:
    bool setInitializedAndReturn(bool value)
    {
        initialized = value;
        return value;
    }

    std::unique_ptr<HardwareMapperInterface> mapper;
    bool initialized;
};

} // namespace blobs
