#pragma once

#include "../firmware_handler.hpp"

#include <cstdint>
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

    /**
     * Attempt to open the file using the specific data interface flag.
     *
     * @param[in] blob - the blob_id to open.
     * @param[in] handlerFlags - the data interface flag, if relevant.
     * @return the session id on success.
     * @throws BlobException on failure.
     */
    virtual std::uint16_t
        openBlob(const std::string& id,
                 blobs::FirmwareBlobHandler::UpdateFlags handlerFlags) = 0;
};
