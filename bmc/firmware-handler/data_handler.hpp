#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace ipmi_flash
{

/**
 * Each data transport mechanism must implement the DataInterface.
 */
class DataInterface
{
  public:
    virtual ~DataInterface() = default;

    /**
     * Initialize data transport mechanism.  Calling this should be idempotent
     * if possible.
     *
     * @return true if successful
     */
    virtual bool open() = 0;

    /**
     * Close the data transport mechanism.
     *
     * @return true if successful
     */
    virtual bool close() = 0;

    /**
     * Copy bytes from external interface (blocking call).
     *
     * @param[in] length - number of bytes to copy
     * @return the bytes read
     */
    virtual std::vector<std::uint8_t> copyFrom(std::uint32_t length) = 0;

    /**
     * set configuration.
     *
     * @param[in] configuration - byte vector of data.
     * @return bool - returns true on success.
     */
    virtual bool writeMeta(const std::vector<std::uint8_t>& configuration) = 0;

    /**
     * read configuration.
     *
     * @return bytes - whatever bytes are required configuration information for
     * the mechanism.
     */
    virtual std::vector<std::uint8_t> readMeta() = 0;
};

struct DataHandlerPack
{
    std::uint16_t bitmask;
    std::unique_ptr<DataInterface> handler;

    DataHandlerPack(std::uint16_t bitmask,
                    std::unique_ptr<DataInterface> handler) :
        bitmask(bitmask), handler(std::move(handler))
    {}

    /* Don't allow copying, assignment or move assignment, only moving. */
    DataHandlerPack(const DataHandlerPack&) = delete;
    DataHandlerPack& operator=(const DataHandlerPack&) = delete;
    DataHandlerPack(DataHandlerPack&&) = default;
    DataHandlerPack& operator=(DataHandlerPack&&) = delete;
};

} // namespace ipmi_flash
