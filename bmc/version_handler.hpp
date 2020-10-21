#pragma once
#include "data_handler.hpp"
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
namespace ipmi_blob_version
{
struct FileActionPack
{
  /** Only file operation action supported currently */
  std::unique_ptr<TriggerableActionInterface> onOpen;
}
using FileActionMap = std::unordered_map<std::string, std::unique_ptr<FileActionPack>>;
class VersionBlobHandler: public blobs::GenericBlobInterface
{
 public:
    enum class VersionBlobState
    {
      /** Nothing has been done yet. A blob read is not allowed */
        notYetStarted = 0,
        /** Version reading process has started. A blob read is not allowed */
        versionReadingStarted,
        /** Version reading process has completed. A blob read is now allowed */
        versionReadingCompleted,
    };

    static std::unique_ptr<blobs::GenericBlobInterface>
        CreateVersionBlobHandler(
            std::vector<std::string>&& blobIds, FileActionMap&& fileActions);

    /**
     * Create a FirmwareBlobHandler.
     *
     * @param[in] blobs - list of blobs_ids to support
     * @param[in] actions - a map of blobId to FileActionPack
     */
    VersionBlobHandler(std::vector<string>&& blobs, FileActionMap&& actions) :
        blobIDs(blobs), lookup(), state(UpdateState::notYetStarted),
        fileActionPacks(std::move(actions))
    {}
    ~VersionBlobHandler() = default;
    VersionBlobHandler(const VersionBlobHandler&) = delete;
    VersionBlobHandler& operator=(const VersionBlobHandler&) = delete;
    VersionBlobHandler(VersionBlobHandler&&) = default;
    VersionBlobHandler& operator=(VersionBlobHandler&&) = default;
 private:
    std::string _activeBlobId;
    blobs::StateFlags _blobState;
    FileActionMap fileActionPacks;
}
} // namespace ipmi_blob_version
