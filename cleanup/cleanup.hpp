#pragma once

#include "fs.hpp"

#include <blobs-ipmid/blobs.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ipmi_flash
{

class FileCleanupHandler : public blobs::GenericBlobInterface
{
  public:
    static std::unique_ptr<blobs::GenericBlobInterface> CreateCleanupHandler(
        const std::string& blobId, const std::vector<std::string>& files,
        std::unique_ptr<FileSystemInterface> helper);

    FileCleanupHandler(const std::string& blobId,
                       const std::vector<std::string>& files,
                       std::unique_ptr<FileSystemInterface> helper) :
        supported(blobId), files(files), helper(std::move(helper))
    {}

    ~FileCleanupHandler() = default;
    FileCleanupHandler(const FileCleanupHandler&) = delete;
    FileCleanupHandler& operator=(const FileCleanupHandler&) = delete;
    FileCleanupHandler(FileCleanupHandler&&) = default;
    FileCleanupHandler& operator=(FileCleanupHandler&&) = default;

    bool canHandleBlob(const std::string& path) override;
    std::vector<std::string> getBlobIds() override;
    bool commit(uint16_t session, const std::vector<uint8_t>& data) override;

    /* These methods return true without doing anything. */
    bool open(uint16_t, uint16_t, const std::string&) override
    {
        return true;
    }
    bool close(uint16_t) override
    {
        return true;
    }
    bool expire(uint16_t) override
    {
        return true;
    }

    /* These methods are unsupported. */
    bool deleteBlob(const std::string&) override
    {
        return false;
    }
    bool stat(const std::string&, blobs::BlobMeta*) override
    {
        return false;
    }
    std::vector<uint8_t> read(uint16_t, uint32_t, uint32_t) override
    {
        return {};
    }
    bool write(uint16_t, uint32_t, const std::vector<uint8_t>&) override
    {
        return false;
    }
    bool writeMeta(uint16_t, uint32_t, const std::vector<uint8_t>&) override
    {
        return false;
    }
    bool stat(uint16_t, blobs::BlobMeta*) override
    {
        return false;
    }

  private:
    std::string supported;
    std::vector<std::string> files;
    std::unique_ptr<FileSystemInterface> helper;
};

} // namespace ipmi_flash
