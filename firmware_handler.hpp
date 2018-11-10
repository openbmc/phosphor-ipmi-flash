#pragma once

#include "data_handler.hpp"
#include "image_handler.hpp"

#include <blobs-ipmid/blobs.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

/**
 * Representation of a session, includes how to read/write data.
 */
struct Session
{
    /** Pointer to the correct Data handler interface. (nullptr on BT (or KCS))
     */
    DataInterface* dataHandler;

    /** Pointer to the correct image handler interface.  (nullptr on hash
     * blob_id) */
    ImageHandlerInterface* imageHandler;

    /** The flags used to open the session. */
    std::uint16_t flags;

    /** A sesion can be for an image (or tarball) or the hash. */
    enum SessionState
    {
        open = 0,
        closed = 1,
    };

    /** The current state of this session. */
    SessionState state;
};

struct ExtChunkHdr
{
    std::uint32_t length; /* Length of the data queued (little endian). */
} __attribute__((packed));

/**
 * Register only one firmware blob handler that will manage all sessions.
 */
class FirmwareBlobHandler : public GenericBlobInterface
{
  public:
    enum UpdateFlags : std::uint16_t
    {
        ipmi = (1 << 8), /* Expect to send contents over IPMI BlockTransfer. */
        p2a = (1 << 9),  /* Expect to send contents over P2A bridge. */
        lpc = (1 << 10), /* Expect to send contents over LPC bridge. */
    };

    /** The state of the firmware update process. */
    enum UpdateState
    {
        /** The initial state. */
        notYetStarted = 0,
        /**
         * The upload process has started, but verification has not started.
         */
        uploadInProgress = 1,
        /** The verification process has started, no more writes allowed. */
        verificationStarted = 2,
        /** The verification process has completed. */
        verificationCompleted = 3,
    };

    /** The return values for verification. */
    enum VerifyCheckResponses
    {
        running = 0,
        success = 1,
        failed = 2,
        other = 3,
    };

    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] firmwares - list of firmware blob_ids to support.
     * @param[in] transports - list of transports to support.
     */
    static std::unique_ptr<GenericBlobInterface> CreateFirmwareBlobHandler(
        const std::vector<HandlerPack>& firmwares,
        const std::vector<DataHandlerPack>& transports);

    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] firmwares - list of firmware types and their handlers
     * @param[in] blobs - list of blobs_ids to support
     * @param[in] transports - list of transport types and their handlers
     * @param[in] bitmask - bitmask of transports to support
     */
    FirmwareBlobHandler(const std::vector<HandlerPack>& firmwares,
                        const std::vector<std::string>& blobs,
                        const std::vector<DataHandlerPack>& transports,
                        std::uint16_t bitmask) :
        handlers(firmwares),
        blobIDs(blobs), transports(transports), bitmask(bitmask), activeImage(),
        activeHash(), lookup(), state(UpdateState::notYetStarted)
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

    /** Allow grabbing the current state. */
    UpdateState getCurrentState() const
    {
        return state;
    };

  private:
    /** List of handlers by type. */
    std::vector<HandlerPack> handlers;

    /** Active list of blobIDs. */
    std::vector<std::string> blobIDs;

    /** List of handlers by transport type. */
    std::vector<DataHandlerPack> transports;

    /** The bits set indicate what transport mechanisms are supported. */
    std::uint16_t bitmask;

    /** Active image session. */
    Session activeImage;

    /** Active hash session. */
    Session activeHash;

    /** A quick method for looking up a session's mechanisms and details. */
    std::map<std::uint16_t, Session*> lookup;

    /** The firmware update state. */
    UpdateState state;

    /** Temporary variable to track whether a blob is open. */
    bool fileOpen = false;
};

} // namespace blobs
