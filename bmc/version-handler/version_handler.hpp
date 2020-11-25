#pragma once
#include "buildjson.hpp"
#include "image_handler.hpp"
#include "status.hpp"
#include "util.hpp"

#include <blobs-ipmid/blobs.hpp>

#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace ipmi_flash
{
struct VersionActionPack
{
  public:
    VersionActionPack(std::unique_ptr<TriggerableActionInterface> openAction) :
        onOpen(std::move(openAction)){};
    VersionActionPack() = default;
    /** Only file operation action supported currently */
    std::unique_ptr<TriggerableActionInterface> onOpen;
};

/*
 * All the information associated with a version blob
 */
struct VersionInfoPack
{
  public:
    VersionInfoPack(const std::string& blobId,
                    std::unique_ptr<VersionActionPack> actionPack,
                    std::unique_ptr<ImageHandlerInterface> imageHandler) :
        _blobId(blobId),
        _actionPack(std::move(actionPack)),
        _imageHandler(std::move(imageHandler)){};
    VersionInfoPack() = default;

    std::string _blobId;
    std::unique_ptr<VersionActionPack> _actionPack;
    std::unique_ptr<ImageHandlerInterface> _imageHandler;
    blobs::StateFlags _blobState;
};

using VersionInfoMap = std::unordered_map<std::string, VersionInfoPack>;
class VersionBlobHandler : public blobs::GenericBlobInterface
{
  public:
    static std::unique_ptr<blobs::GenericBlobInterface>
        create(VersionInfoMap&& versionMap);
    /**
     * Create a VersionBlobHandler.
     *
     * @param[in] blobs - list of blobs_ids to support
     * @param[in] actions - a map of blobId to VersionInfoPack
     */
    VersionBlobHandler(std::vector<std::string>&& blobs,
                       VersionInfoMap&& handlerMap) :
        _blobIds(blobs),
        _versionInfoMap(std::move(handlerMap))
    {}
    ~VersionBlobHandler() = default;
    VersionBlobHandler(const VersionBlobHandler&) = delete;
    VersionBlobHandler& operator=(const VersionBlobHandler&) = delete;
    VersionBlobHandler(VersionBlobHandler&&) = default;
    VersionBlobHandler& operator=(VersionBlobHandler&&) = default;
    bool canHandleBlob(const std::string& path) override;
    std::vector<std::string> getBlobIds() override;
    bool deleteBlob(const std::string& path) override;
    bool stat(const std::string&, blobs::BlobMeta* meta) override;
    bool open(uint16_t session, uint16_t flags,
              const std::string& path) override;
    std::vector<uint8_t> read(uint16_t session, uint32_t offset,
                              uint32_t requestedSize) override;
    bool write(uint16_t session, uint32_t offset,
               const std::vector<uint8_t>& data) override
    {
        return false; /* not supported */
    };
    bool writeMeta(uint16_t session, uint32_t offset,
                   const std::vector<uint8_t>& data) override
    {
        return false; /* not supported */
    }
    bool commit(uint16_t session, const std::vector<uint8_t>& data) override
    {
        return false; // not supported
    }
    bool close(uint16_t session) override;
    bool stat(uint16_t session, blobs::BlobMeta* meta) override;
    bool expire(uint16_t session) override;
    bool cleanup(uint16_t session);

  private:
    std::vector<std::string> _blobIds;
    VersionInfoMap _versionInfoMap;
    std::unordered_map<uint16_t, std::string> _sessionToBlob;
};
} // namespace ipmi_flash
