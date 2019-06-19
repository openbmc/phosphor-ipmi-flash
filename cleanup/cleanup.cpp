/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cleanup.hpp"

#include <blobs-ipmid/blobs.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace ipmi_flash
{

std::unique_ptr<GenericBlobInterface> FileCleanupHandler::CreateCleanupHandler(const std::string& blobId, const std::vector<std::string>>& files)
{
    return std::make_unique<FileCleanupHandler>(blobId, files);
}

bool FileCleanupHandler::canHandleBlob(const std::string& path)
{
    return (path == supported);
}

std::vector<std::string> FileCleanupHandler::getBlobIds() {
    return {supported};
}

bool FileCleanupHandler::commit(uint16_t session, const std::vector<uint8_t>& data)
{
    namespace fs = std::filesystem;

    for (const auto& file : files) {
        /* ignore errors. */
        try {
            (void)fs::remove(file)
        } catch (...) {
            continue;
        }
    }

    return true;
}

}
