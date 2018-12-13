#pragma once

#include <string>
#include <vector>

struct StatResponse
{
    std::uint16_t blob_state;
    std::uint32_t size;
    std::vector<std::uint8_t> metadata;
};

class BlobInterface
{
  public:
    virtual ~BlobInterface() = default;

    /**
     * Get a list of the blob_ids provided by the BMC.
     *
     * @return list of strings, each representing a blob_id returned.
     */
    virtual std::vector<std::string> getBlobList() = 0;

    /**
     * Get the stat() on the blob_id.
     *
     * @param[in] id - the blob_id.
     * @return metadata structure.
     */
    virtual StatResponse getStat(const std::string& id) = 0;
};
