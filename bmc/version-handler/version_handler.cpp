#include "version_handler.hpp"
/**
 * factory to create a BlobHandler for use by phosphor-ipmi-blobs
 */
namespace ipmi_flash
{
std::unique_ptr<blobs::GenericBlobInterface>
    VersionBlobHandler::create(VersionInfoMap&& versionMap)
{
    if (versionMap.empty())
    {
        return nullptr;
    }
    std::vector<std::string> blobList;
    for (const auto& [key, val] : versionMap)
    {
        if (val._blobId != key || val._actionPack == nullptr ||
            val._imageHandler == nullptr || val._actionPack->onOpen == nullptr)
        {
            return nullptr;
        }
        blobList.push_back(key);
    }
    return std::make_unique<VersionBlobHandler>(std::move(blobList),
                                                std::move(versionMap));
}

bool VersionBlobHandler::canHandleBlob(const std::string& path)
{
    return (std::find(_blobIds.begin(), _blobIds.end(), path) !=
            _blobIds.end());
}

std::vector<std::string> VersionBlobHandler::getBlobIds()
{
    return _blobIds;
}

/**
 * deleteBlob - does nothing, always fails
 */
bool VersionBlobHandler::deleteBlob(const std::string& path)
{
    return false;
}

bool VersionBlobHandler::stat(const std::string& path, blobs::BlobMeta* meta)
{
    // TODO: stat should return the blob state and in the meta data information
    // on whether a read is successful should be contained
    // do things like determine if systemd target is triggered
    // then check if file can be opened for read
    return false; /* not yet implemented */
}

bool VersionBlobHandler::open(uint16_t session, uint16_t flags,
                              const std::string& path)
{
    if (_sessionToBlob.insert({session, path}).second == false)
    {
        fprintf(stderr, "open %s fail: session number %d assigned to %s\n",
                path.c_str(), session, _sessionToBlob.at(session).c_str());
        return false;
    }
    /* only reads are supported, check if blob is handled and make sure
     * the blob isn't already opened
     */
    if (flags != blobs::read)
    {
        fprintf(stderr, "open %s fail: unsupported flags(0x%04X.)\n",
                path.c_str(), flags);
        cleanup(session);
        return false;
    }
    if (!canHandleBlob(path))
    {
        fprintf(stderr, "open %s fail: unrecognized blob\n", path.c_str());
        cleanup(session);
        return false;
    }

    try
    {
        auto& v = _versionInfoMap.at(path);
        if (v._blobState == blobs::StateFlags::open_read)
        {
            cleanup(session);
            return false;
        }
        if (v._actionPack->onOpen->trigger() == false)
        {
            fprintf(stderr, "open %s fail: onOpen trigger failed\n",
                    path.c_str());
            cleanup(session);
            return false;
        }
        v._blobState = blobs::StateFlags::open_read;
        return true;
    }
    catch (const std::out_of_range& e)
    {
        fprintf(stderr, "open %s fail, exception:%s\n", path.c_str(), e.what());
        cleanup(session);
        return false;
    }
}

std::vector<uint8_t> VersionBlobHandler::read(uint16_t session, uint32_t offset,
                                              uint32_t requestedSize)
{
    try
    {
        const auto& blobName = _sessionToBlob.at(session);
        auto& pack = _versionInfoMap.at(blobName);
        /* onOpen trigger must be successful, otherwise potential
         * for stale data to be read
         */
        if (pack._actionPack->onOpen->status() != ActionStatus::success)
        {
          fprintf(stderr, "read failed: onOpen trigger not successful\n");
            return {};
        }
        if(!pack._imageHandler->open("don't care", std::ios::in))
        {
            fprintf(stderr,
                    "read failed: unable file open unsuccessful blob=%s\n",
                    blobName.c_str());
            return {};
        }
        auto d = pack._imageHandler->read(offset, requestedSize);
        if (!d)
        {
            fprintf(stderr, "read failed: unable to read file for blob %s\n",
                    blobName.c_str());
            pack._imageHandler->close();
            return {};
        }
        pack._imageHandler->close();
        return *d;
    }
    catch (const std::out_of_range& e)
    {
        return {};
    }
}

bool VersionBlobHandler::close(uint16_t session)
{
    return cleanup(session);
}

bool VersionBlobHandler::stat(uint16_t session, blobs::BlobMeta* meta)
{
    return false;
}

bool VersionBlobHandler::expire(uint16_t session)
{
    return cleanup(session);
}

bool VersionBlobHandler::cleanup(uint16_t session)
{
    try
    {
        const auto& blobName = _sessionToBlob.at(session);
        auto& pack = _versionInfoMap.at(blobName);
        if (pack._actionPack->onOpen->status() == ActionStatus::running)
        {
            pack._actionPack->onOpen->abort();
        }
        pack._blobState = static_cast<blobs::StateFlags>(0);
        _sessionToBlob.erase(session);
        return true;
    }
    catch (const std::out_of_range& e)
    {
        return false;
    }
}
} // namespace ipmi_flash
