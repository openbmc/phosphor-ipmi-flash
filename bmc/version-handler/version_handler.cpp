#include "version_handler.hpp"

#include <stdexcept>
#include <utility>
#include <vector>

namespace ipmi_flash
{

VersionBlobHandler::VersionBlobHandler(
    std::vector<HandlerConfig<ActionPack>>&& configs)
{
    for (auto& config : configs)
    {
        auto info = std::make_unique<BlobInfo>();
        info->blobId = std::move(config.blobId);
        info->actions = std::move(config.actions);
        info->handler = std::move(config.handler);
        if (!blobInfoMap.try_emplace(info->blobId, std::move(info)).second)
        {
            fprintf(stderr, "Ignoring duplicate config for %s\n",
                    info->blobId.c_str());
        }
    }
}

bool VersionBlobHandler::canHandleBlob(const std::string& path)
{
    return blobInfoMap.find(path) != blobInfoMap.end();
}

std::vector<std::string> VersionBlobHandler::getBlobIds()
{
    std::vector<std::string> ret;
    for (const auto& [key, _] : blobInfoMap)
    {
        ret.emplace_back(key);
    }
    return ret;
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
    /* only reads are supported, check if blob is handled and make sure
     * the blob isn't already opened
     */
    if (flags != blobs::read)
    {
        fprintf(stderr, "open %s fail: unsupported flags(0x%04X.)\n",
                path.c_str(), flags);
        return false;
    }

    auto& v = *blobInfoMap.at(path);
    sessionToBlob[session] = &v;

    if (v.blobState == blobs::StateFlags::open_read)
    {
        fprintf(stderr, "open %s fail: blob already opened for read\n",
                path.c_str());
        cleanup(session);
        return false;
    }
    if (v.actions->onOpen->trigger() == false)
    {
        fprintf(stderr, "open %s fail: onOpen trigger failed\n", path.c_str());
        cleanup(session);
        return false;
    }
    v.blobState = blobs::StateFlags::open_read;
    return true;
}

std::vector<uint8_t> VersionBlobHandler::read(uint16_t session, uint32_t offset,
                                              uint32_t requestedSize)
{
    BlobInfo* pack;
    try
    {
        pack = sessionToBlob.at(session);
    }
    catch (const std::out_of_range& e)
    {
        return {};
    }
    /* onOpen trigger must be successful, otherwise potential
     * for stale data to be read
     */
    if (pack->actions->onOpen->status() != ActionStatus::success)
    {
        fprintf(stderr, "read failed: onOpen trigger not successful\n");
        return {};
    }
    if (!pack->handler->open("don't care", std::ios::in))
    {
        fprintf(stderr, "read failed: file open unsuccessful blob=%s\n",
                pack->blobId.c_str());
        return {};
    }
    auto d = pack->handler->read(offset, requestedSize);
    if (!d)
    {
        fprintf(stderr, "read failed: unable to read file for blob %s\n",
                pack->blobId.c_str());
        pack->handler->close();
        return {};
    }
    pack->handler->close();
    return *d;
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
        auto& pack = *sessionToBlob.at(session);
        if (pack.actions->onOpen->status() == ActionStatus::running)
        {
            pack.actions->onOpen->abort();
        }
        pack.blobState = static_cast<blobs::StateFlags>(0);
        sessionToBlob.erase(session);
        return true;
    }
    catch (const std::out_of_range& e)
    {
        return false;
    }
}
} // namespace ipmi_flash
