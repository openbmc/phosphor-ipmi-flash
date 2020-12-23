#pragma once
#include "buildjson.hpp"
#include "image_handler.hpp"
#include "status.hpp"
#include "util.hpp"

#include <blobs-ipmid/blobs.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ipmi_flash
{

class VersionBlobHandler : public blobs::GenericBlobInterface
{
  public:
    struct ActionPack
    {
        /** Only file operation action supported currently */
        std::unique_ptr<TriggerableActionInterface> onOpen;
    };

    /**
     * Create a VersionBlobHandler.
     *
     * @param[in] configs - list of blob configurations to support
     */
    VersionBlobHandler(std::vector<HandlerConfig<ActionPack>>&& configs);

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
    struct BlobInfo
    {
        Pinned<std::string> blobId;
        std::unique_ptr<ActionPack> actions;
        std::unique_ptr<ImageHandlerInterface> handler;
        blobs::StateFlags blobState = static_cast<blobs::StateFlags>(0);
    };

    std::unordered_map<std::string_view, std::unique_ptr<BlobInfo>> blobInfoMap;
    std::unordered_map<uint16_t, BlobInfo*> sessionToBlob;
};
} // namespace ipmi_flash
