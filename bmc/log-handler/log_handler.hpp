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

#pragma once
#include "handler_config.hpp"
#include "image_handler.hpp"
#include "status.hpp"
#include "util.hpp"

#include <blobs-ipmid/blobs.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ipmi_flash
{

class LogBlobHandler : public blobs::GenericBlobInterface
{
  public:
    struct ActionPack
    {
        /** Only file operation action supported currently */
        std::unique_ptr<TriggerableActionInterface> onOpen;
        std::unique_ptr<TriggerableActionInterface> onDelete;
    };

    /**
     * Create a LogBlobHandler.
     *
     * @param[in] configs - list of blob configurations to support
     */
    LogBlobHandler(std::vector<HandlerConfig<ActionPack>>&& configs);

    ~LogBlobHandler() = default;
    LogBlobHandler(const LogBlobHandler&) = delete;
    LogBlobHandler& operator=(const LogBlobHandler&) = delete;
    LogBlobHandler(LogBlobHandler&&) = default;
    LogBlobHandler& operator=(LogBlobHandler&&) = default;

    bool canHandleBlob(const std::string& path) override;
    std::vector<std::string> getBlobIds() override;
    bool deleteBlob(const std::string& path) override;
    bool stat(const std::string&, blobs::BlobMeta* meta) override;
    bool open(uint16_t session, uint16_t flags,
              const std::string& path) override;
    std::vector<uint8_t> read(uint16_t session, uint32_t offset,
                              uint32_t requestedSize) override;
    bool write(uint16_t, uint32_t, const std::vector<uint8_t>&) override
    {
        return false; /* not supported */
    };
    bool writeMeta(uint16_t, uint32_t, const std::vector<uint8_t>&) override
    {
        return false; /* not supported */
    }
    bool commit(uint16_t, const std::vector<uint8_t>&) override
    {
        return false; // not supported
    }
    bool close(uint16_t session) override;
    bool stat(uint16_t session, blobs::BlobMeta* meta) override;
    bool expire(uint16_t session) override;

  private:
    struct SessionInfo;

    struct BlobInfo
    {
        Pinned<std::string> blobId;
        std::unique_ptr<ActionPack> actions;
        std::unique_ptr<ImageHandlerInterface> handler;
        std::set<SessionInfo*> sessionsToUpdate;
    };

    struct SessionInfo
    {
        BlobInfo* blob;

        // A cached copy of the version data shared by all clients for a single
        // execution of the version retrieval action. This is is null until the
        // TriggerableAction has completed. If the action is an error, the
        // shared object is nullopt. Otherwise, contains a vector of the version
        // data when successfully read.
        std::shared_ptr<const std::optional<std::vector<uint8_t>>> data;
    };

    std::unordered_map<std::string_view, std::unique_ptr<BlobInfo>> blobInfoMap;
    std::unordered_map<uint16_t, std::unique_ptr<SessionInfo>> sessionInfoMap;
};

} // namespace ipmi_flash
