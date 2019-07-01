#pragma once

#include "config.h"

#include "data_handler.hpp"
#include "image_handler.hpp"
#include "status.hpp"
#include "util.hpp"

#include <algorithm>
#include <blobs-ipmid/blobs.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ipmi_flash
{

/**
 * Given a firmware name, provide a set of triggerable action interfaces
 * associated with that firmware type.
 */
struct ActionPack
{
    /** The name of the action pack, something like image, or tarball, or bios.
     * The firmware blob id is parsed to pull the "filename" portion from the
     * path, and matched against this field.
     */
    const std::string name;
    std::unique_ptr<TriggerableActionInterface> preparation;
    std::unique_ptr<TriggerableActionInterface> verification;
    std::unique_ptr<TriggerableActionInterface> update;
};

/**
 * Representation of a session, includes how to read/write data.
 */
struct Session
{
    /**
     * Built a session object.
     *
     * @param[in] the active path to which this corresponds.
     */
    explicit Session(const std::string& path) :
        dataHandler(nullptr), imageHandler(nullptr), flags(0), activePath(path)
    {
    }

    /**
     * Pointer to the correct Data handler interface. (nullptr on BT (or KCS))
     */
    DataInterface* dataHandler;

    /**
     * Pointer to the correct image handler interface.  (nullptr on hash
     * blob_id)
     */
    ImageHandlerInterface* imageHandler;

    /** The flags used to open the session. */
    std::uint16_t flags;

    /** The active path. */
    std::string activePath;
};

/**
 * Register only one firmware blob handler that will manage all sessions.
 */
class FirmwareBlobHandler : public blobs::GenericBlobInterface
{
  public:
    /** The state of the firmware update process. */
    enum class UpdateState
    {
        /** The initial state. */
        notYetStarted = 0,
        /** The BMC is expecting to receive bytes. */
        uploadInProgress,
        /** The BMC is ready for verification or more bytes. */
        verificationPending,
        /** The verification process has started, no more writes allowed. */
        verificationStarted,
        /** The verification process has completed. */
        verificationCompleted,
        /** The update process is pending. */
        updatePending,
        /** The update process has started. */
        updateStarted,
        /** The update has completed (optional state to reach) */
        updateCompleted,
    };

    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] firmwares - list of firmware blob_ids to support.
     * @param[in] transports - list of transports to support.
     * @param[in] verification - pointer to object for triggering verification
     * @param[in] update - point to object for triggering the update
     */
    static std::unique_ptr<blobs::GenericBlobInterface>
        CreateFirmwareBlobHandler(
            const std::vector<HandlerPack>& firmwares,
            const std::vector<DataHandlerPack>& transports,
            std::unique_ptr<TriggerableActionInterface> preparation,
            std::unique_ptr<TriggerableActionInterface> verification,
            std::unique_ptr<TriggerableActionInterface> update);

    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] firmwares - list of firmware types and their handlers
     * @param[in] blobs - list of blobs_ids to support
     * @param[in] transports - list of transport types and their handlers
     * @param[in] bitmask - bitmask of transports to support
     * @param[in] verification - pointer to object for triggering verification
     * @param[in] update - point to object for triggering the update
     */
    FirmwareBlobHandler(
        const std::vector<HandlerPack>& firmwares,
        const std::vector<std::string>& blobs,
        const std::vector<DataHandlerPack>& transports, std::uint16_t bitmask,
        std::unique_ptr<TriggerableActionInterface> preparation,
        std::unique_ptr<TriggerableActionInterface> verification,
        std::unique_ptr<TriggerableActionInterface> update) :
        handlers(firmwares),
        blobIDs(blobs), transports(transports), bitmask(bitmask),
        activeImage(activeImageBlobId), activeHash(activeHashBlobId),
        verifyImage(verifyBlobId), updateImage(updateBlobId), lookup(),
        state(UpdateState::notYetStarted), preparation(std::move(preparation)),
        verification(std::move(verification)), update(std::move(update))
    {
    }
    ~FirmwareBlobHandler() = default;
    FirmwareBlobHandler(const FirmwareBlobHandler&) = delete;
    FirmwareBlobHandler& operator=(const FirmwareBlobHandler&) = delete;
    FirmwareBlobHandler(FirmwareBlobHandler&&) = default;
    FirmwareBlobHandler& operator=(FirmwareBlobHandler&&) = default;

    bool canHandleBlob(const std::string& path) override;
    std::vector<std::string> getBlobIds() override;
    bool deleteBlob(const std::string& path) override;
    bool stat(const std::string& path, blobs::BlobMeta* meta) override;
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
    bool stat(uint16_t session, blobs::BlobMeta* meta) override;
    bool expire(uint16_t session) override;

    void abortProcess();

    void abortVerification();
    bool triggerVerification();
    void abortUpdate();
    bool triggerUpdate();

    /** Allow grabbing the current state. */
    UpdateState getCurrentState() const
    {
        return state;
    };

    /** Provide for any state change triggers in convenience handler. */
    void changeState(UpdateState next);

  private:
    void addBlobId(const std::string& blob)
    {
        auto blobIdMatch = std::find_if(
            blobIDs.begin(), blobIDs.end(),
            [&blob](const std::string& iter) { return (iter == blob); });
        if (blobIdMatch == blobIDs.end())
        {
            blobIDs.push_back(blob);
        }
    }

    void removeBlobId(const std::string& blob)
    {
        blobIDs.erase(std::remove(blobIDs.begin(), blobIDs.end(), blob),
                      blobIDs.end());
    }

    ActionStatus getVerifyStatus();
    ActionStatus getActionStatus();

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

    /** Session for verification. */
    Session verifyImage;

    /** Session for update. */
    Session updateImage;

    /** A quick method for looking up a session's mechanisms and details. */
    std::map<std::uint16_t, Session*> lookup;

    /** The firmware update state. */
    UpdateState state;

    /* preparation is triggered once we go into uploadInProgress(), but only
     * once per full cycle, going back to notYetStarted resets this.
     */
    bool preparationTriggered = false;
    std::unique_ptr<TriggerableActionInterface> preparation;

    std::unique_ptr<TriggerableActionInterface> verification;

    std::unique_ptr<TriggerableActionInterface> update;

    /** Temporary variable to track whether a blob is open. */
    bool fileOpen = false;

    ActionStatus lastVerificationStatus = ActionStatus::unknown;

    ActionStatus lastUpdateStatus = ActionStatus::unknown;
};

} // namespace ipmi_flash
