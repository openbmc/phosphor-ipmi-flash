#include "version_handler.hpp"

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
        info->actions->onOpen->setCallback(
            [infoP = info.get()](TriggerableActionInterface& tai) {
                auto data =
                    std::make_shared<std::optional<std::vector<uint8_t>>>();
                do
                {
                    if (tai.status() != ActionStatus::success)
                    {
                        fprintf(stderr, "Version file unit failed for %s\n",
                                infoP->blobId.c_str());
                        continue;
                    }
                    if (!infoP->handler->open("", std::ios::in))
                    {
                        fprintf(stderr, "Opening version file failed for %s\n",
                                infoP->blobId.c_str());
                        continue;
                    }
                    auto d = infoP->handler->read(
                        0, std::numeric_limits<uint32_t>::max());
                    infoP->handler->close();
                    if (!d)
                    {
                        fprintf(stderr, "Reading version file failed for %s\n",
                                infoP->blobId.c_str());
                        continue;
                    }
                    *data = std::move(d);
                } while (false);
                for (auto sessionP : infoP->sessionsToUpdate)
                {
                    sessionP->data = data;
                }
                infoP->sessionsToUpdate.clear();
            });
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
    try
    {
        auto& v = blobInfoMap.at(path);
        meta->blobState =
            v->sessionsToUpdate.empty() ? 0 : blobs::StateFlags::open_read;
        meta->size = v->handler->getSize();
        meta->metadata.clear();
        if (meta->blobState == blobs::StateFlags::open_read)
        {
            auto status = v->actions->onOpen->status();
            meta->metadata.push_back(static_cast<std::uint8_t>(status));
            if (status == ActionStatus::success)
            {
                meta->blobState = blobs::StateFlags::committed;
            }
            else if (status == ActionStatus::running)
            {
                meta->blobState = blobs::StateFlags::committing;
            }
            else if (status == ActionStatus::failed)
            {
                meta->blobState = blobs::StateFlags::commit_error;
            }
            meta->blobState |= blobs::StateFlags::open_read;
        }
        else
        {
            meta->metadata.push_back(
                static_cast<std::uint8_t>(ActionStatus::unknown));
        }
        return true;
    }
    catch (const std::out_of_range& e)
    {
        return false;
    }
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

    auto info = std::make_unique<SessionInfo>();
    info->blob = blobInfoMap.at(path).get();
    info->blob->sessionsToUpdate.emplace(info.get());
    if (info->blob->sessionsToUpdate.size() == 1 &&
        !info->blob->actions->onOpen->trigger())
    {
        fprintf(stderr, "open %s fail: onOpen trigger failed\n", path.c_str());
        info->blob->sessionsToUpdate.erase(info.get());
        return false;
    }

    sessionInfoMap[session] = std::move(info);
    return true;
}

std::vector<uint8_t> VersionBlobHandler::read(uint16_t session, uint32_t offset,
                                              uint32_t requestedSize)
{
    auto& data = sessionInfoMap.at(session)->data;
    if (data == nullptr || !*data || (*data)->size() < offset)
    {
        return {};
    }
    std::vector<uint8_t> ret(
        std::min<size_t>(requestedSize, (*data)->size() - offset));
    std::memcpy(&ret[0], &(**data)[offset], ret.size());
    return ret;
}

bool VersionBlobHandler::close(uint16_t session)
{
    auto it = sessionInfoMap.find(session);
    if (it == sessionInfoMap.end())
    {
        return false;
    }
    auto& info = *it->second;
    info.blob->sessionsToUpdate.erase(&info);
    if (info.blob->sessionsToUpdate.empty())
    {
        info.blob->actions->onOpen->abort();
    }
    sessionInfoMap.erase(it);
    return true;
}

bool VersionBlobHandler::stat(uint16_t session, blobs::BlobMeta* meta)
{
    try
    {
        const auto& blobName = sessionInfoMap.at(session)->blob->blobId;
        return stat(blobName, meta);
    }
    catch (const std::out_of_range& e)
    {
        return false;
    }
}

bool VersionBlobHandler::expire(uint16_t session)
{
    close(session);
    return true;
}

} // namespace ipmi_flash
