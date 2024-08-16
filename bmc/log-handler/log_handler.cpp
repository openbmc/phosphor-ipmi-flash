// Copyright 2021 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "log_handler.hpp"

#include <algorithm>
#include <cstring>
#include <ios>
#include <limits>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace ipmi_flash
{

LogBlobHandler::LogBlobHandler(std::vector<HandlerConfig<ActionPack>>&& configs)
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
                        fprintf(stderr,
                                "LogBlobHandler: Log file unit failed for %s\n",
                                infoP->blobId.c_str());
                        continue;
                    }
                    if (!infoP->handler->open("", std::ios::in))
                    {
                        fprintf(
                            stderr,
                            "LogBlobHandler: Opening log file failed for %s\n",
                            infoP->blobId.c_str());
                        continue;
                    }
                    auto d = infoP->handler->read(
                        0, std::numeric_limits<uint32_t>::max());
                    infoP->handler->close();
                    if (!d)
                    {
                        fprintf(
                            stderr,
                            "LogBlobHandler: Reading log file failed for %s\n",
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
            fprintf(stderr,
                    "LogBlobHandler: Ignoring duplicate config for %s\n",
                    info->blobId.c_str());
        }
    }
}

bool LogBlobHandler::canHandleBlob(const std::string& path)
{
    return blobInfoMap.find(path) != blobInfoMap.end();
}

std::vector<std::string> LogBlobHandler::getBlobIds()
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
bool LogBlobHandler::deleteBlob(const std::string& path)
{
    for (const auto& [sessionId, sessionInfo] : sessionInfoMap)
    {
        if (sessionInfo->blob->blobId == path)
        {
            fprintf(stderr,
                    "LogBlobHandler: delete %s fail: there is an open session "
                    "for this blob\n",
                    path.c_str());
            return false;
        }
    }

    auto* blob = blobInfoMap.at(path).get();
    if (!blob->actions->onDelete->trigger())
    {
        fprintf(stderr,
                "LogBlobHandler: delete %s fail: onDelete trigger failed\n",
                path.c_str());
        return false;
    }
    return true;
}

bool LogBlobHandler::stat(const std::string&, blobs::BlobMeta*)
{
    return false;
}

bool LogBlobHandler::open(uint16_t session, uint16_t flags,
                          const std::string& path)
{
    /* only reads are supported, check if blob is handled and make sure
     * the blob isn't already opened
     */
    if (flags != blobs::read)
    {
        fprintf(stderr,
                "LogBlobHandler: open %s fail: unsupported flags(0x%04X.)\n",
                path.c_str(), flags);
        return false;
    }

    auto info = std::make_unique<SessionInfo>();
    info->blob = blobInfoMap.at(path).get();
    info->blob->sessionsToUpdate.emplace(info.get());
    if (info->blob->sessionsToUpdate.size() == 1 &&
        !info->blob->actions->onOpen->trigger())
    {
        fprintf(stderr, "LogBlobHandler: open %s fail: onOpen trigger failed\n",
                path.c_str());
        info->blob->sessionsToUpdate.erase(info.get());
        return false;
    }

    sessionInfoMap[session] = std::move(info);
    return true;
}

std::vector<uint8_t> LogBlobHandler::read(uint16_t session, uint32_t offset,
                                          uint32_t requestedSize)
{
    auto& data = sessionInfoMap.at(session)->data;
    if (data == nullptr || !*data)
    {
        throw std::runtime_error("LogBlobHandler: Log data not ready for read");
    }
    if ((*data)->size() < offset)
    {
        return {};
    }
    std::vector<uint8_t> ret(
        std::min<size_t>(requestedSize, (*data)->size() - offset));
    std::memcpy(&ret[0], &(**data)[offset], ret.size());
    return ret;
}

bool LogBlobHandler::close(uint16_t session)
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

bool LogBlobHandler::stat(uint16_t session, blobs::BlobMeta* meta)
{
    const auto& data = sessionInfoMap.at(session)->data;
    if (data == nullptr)
    {
        meta->blobState = blobs::StateFlags::committing;
        meta->size = 0;
    }
    else if (!*data)
    {
        meta->blobState = blobs::StateFlags::commit_error;
        meta->size = 0;
    }
    else
    {
        meta->blobState = blobs::StateFlags::committed |
                          blobs::StateFlags::open_read;
        meta->size = (*data)->size();
    }
    return true;
}

bool LogBlobHandler::expire(uint16_t session)
{
    close(session);
    return true;
}

} // namespace ipmi_flash
