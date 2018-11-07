#pragma once

#include "image_handler.hpp"

#include <blobs-ipmid/blobs.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

enum class FirmwareUpdateFlags
{
    bt = (1 << 8),   /* Expect to send contents over IPMI BlockTransfer. */
    p2a = (1 << 9),  /* Expect to send contents over P2A bridge. */
    lpc = (1 << 10), /* Expect to send contents over LPC bridge. */
};

/**
 * Register only one firmware blob handler that will manage all sessions.
 */
class FirmwareBlobHandler : public GenericBlobInterface
{
  public:
    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] firmwares - list of firmware blob_ids to support.
     * @param[in] transports - bitmask of transports to support.
     */
    static std::unique_ptr<GenericBlobInterface>
        CreateFirmwareBlobHandler(const std::vector<HandlerPack>& firmwares,
                                  std::uint16_t transports);

    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] blobs - list of blobs_ids to support and their image handlers.
     * @param[in] transports - bitmask of transports to support.
     */
    FirmwareBlobHandler(const std::vector<HandlerPack>& firmwares,
                        const std::vector<std::string>& blobs,
                        std::uint16_t transports) :
        handlers(firmwares),
        blobIDs(blobs), transports(transports)
    {
    }
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

    static const std::string hashBlobID;
    static const std::string activeImageBlobID;
    static const std::string activeHashBlobID;

  private:
    /** List of handlers by type. */
    std::vector<HandlerPack> handlers;

    /** Active list of blobIDs. */
    std::vector<std::string> blobIDs;

    /** The bits set indicate what transport mechanisms are supported. */
    std::uint16_t transports;

    /** Temporary variable to track whether a blob is open. */
    bool fileOpen = false;
};

} // namespace blobs
