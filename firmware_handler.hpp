#pragma once

#include <blobs-ipmid/blobs.hpp>

namespace blobs
{

/**
 * Register only one firmware blob handler that will manage all sessions.
 */
class FirmwareBlobHandler : public GenericBlobInterface
{
  public:
    FirmwareBlobHandler() = default;
    ~FirmwareBlobHandler() = default;
    FirmwareBlobHandler(const FirmwareBlobHandler&) = default;
    FirmwareBlobHandler& operator=(const FirmwareBlobHandler&) = default;
    FirmwareBlobHandler(FirmwareBlobHandler&&) = default;
    FirmwareBlobHandler& operator=(FirmwareBlobHandler&&) = default;

    bool canHandleBlob(const std::string& path) override;
    std::vector<std::string> getBlobIds() override;
    bool deleteBlob(const std::string& path) override;
    bool stat(const std::string& path, struct BlobMeta* meta) override;
    bool open(uint16_t session, uint16_t flags,
              const std::string& path) override;
    std::vector<uint8_t> read(uint16_t session, uint32_t offset,
                              uint32_t requestedSize) override;
    bool write(uint16_t session, uint32_t offset,
               const std::vector<uint8_t>& data) override;
    bool writeMeta(uint16_t session, uint32_t offset,
                   const std::vector<uint8_t>& data) override;
    bool commit(uint16_t session, const std::vector<uint8_t>& data) override;
    bool close(uint16_t session) override;
    bool stat(uint16_t session, struct BlobMeta* meta) override;
    bool expire(uint16_t session) override;
};

} // namespace blobs
