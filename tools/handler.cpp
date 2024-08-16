/*
 * Copyright 2018 Google Inc.
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

#include "handler.hpp"

#include "flags.hpp"
#include "helper.hpp"
#include "status.hpp"
#include "tool_errors.hpp"
#include "util.hpp"

#include <ipmiblob/blob_errors.hpp>
#include <stdplus/function_view.hpp>
#include <stdplus/handle/managed.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace host_tool
{

static void closeBlob(uint16_t&& session, ipmiblob::BlobInterface*& blob)
{
    blob->closeBlob(session);
}

using BlobHandle =
    stdplus::Managed<uint16_t, ipmiblob::BlobInterface*>::Handle<closeBlob>;

template <typename... Args>
inline BlobHandle openBlob(ipmiblob::BlobInterface* blob, Args&&... args)
{
    return BlobHandle(blob->openBlob(std::forward<Args>(args)...), blob);
}

bool UpdateHandler::checkAvailable(const std::string& goalFirmware)
{
    std::vector<std::string> blobs = blob->getBlobList();

    auto blobInst = std::find_if(
        blobs.begin(), blobs.end(), [&goalFirmware](const std::string& iter) {
            /* Running into weird scenarios where the string comparison doesn't
             * work.  TODO: revisit.
             */
            return (0 == std::memcmp(goalFirmware.c_str(), iter.c_str(),
                                     goalFirmware.length()));
            // return (goalFirmware.compare(iter));
        });
    if (blobInst == blobs.end())
    {
        std::fprintf(stderr, "%s not found\n", goalFirmware.c_str());
        return false;
    }

    return true;
}

std::vector<uint8_t> UpdateHandler::retryIfFailed(
    stdplus::function_view<std::vector<uint8_t>()> callback)
{
    constexpr uint8_t retryCount = 3;
    uint8_t i = 1;
    while (true)
    {
        try
        {
            return callback();
        }
        catch (const ipmiblob::BlobException& b)
        {
            throw ToolException(
                "blob exception received: " + std::string(b.what()));
        }
        catch (const ToolException& t)
        {
            uint8_t remains = retryCount - i;
            std::fprintf(
                stderr,
                "tool exception received: %s: Retrying it %u more times\n",
                t.what(), remains);
            if (remains == 0)
                throw;
        }
        ++i;
        handler->waitForRetry();
    }
    return {};
}

void UpdateHandler::retrySendFile(const std::string& target,
                                  const std::string& path)
{
    auto supported = handler->supportedType();
    auto session =
        openBlob(blob, target,
                 static_cast<std::uint16_t>(supported) |
                     static_cast<std::uint16_t>(
                         ipmi_flash::FirmwareFlags::UpdateFlags::openWrite));

    if (!handler->sendContents(path, *session))
    {
        throw ToolException("Failed to send contents of " + path);
    }
}

void UpdateHandler::sendFile(const std::string& target, const std::string& path)
{
    retryIfFailed([this, target, path]() {
        this->retrySendFile(target, path);
        return std::vector<uint8_t>{};
    });
}

void UpdateHandler::retryVerifyFile(const std::string& target,
                                    bool ignoreStatus)
{
    auto session =
        openBlob(blob, target,
                 static_cast<std::uint16_t>(
                     ipmi_flash::FirmwareFlags::UpdateFlags::openWrite));

    std::fprintf(stderr, "Committing to %s to trigger service\n",
                 target.c_str());
    blob->commit(*session, {});

    if (ignoreStatus)
    {
        // Skip checking the blob for status if ignoreStatus is enabled
        return;
    }

    std::fprintf(stderr, "Calling stat on %s session to check status\n",
                 target.c_str());
    pollStatus(*session, blob);
    return;
}

bool UpdateHandler::verifyFile(const std::string& target, bool ignoreStatus)
{
    retryIfFailed([this, target, ignoreStatus]() {
        this->retryVerifyFile(target, ignoreStatus);
        return std::vector<uint8_t>{};
    });

    return true;
}

std::vector<uint8_t>
    UpdateHandler::retryReadVersion(const std::string& versionBlob)
{
    auto session =
        openBlob(blob, versionBlob,
                 static_cast<std::uint16_t>(
                     ipmi_flash::FirmwareFlags::UpdateFlags::openRead));

    std::fprintf(stderr, "Calling stat on %s session to check status\n",
                 versionBlob.c_str());

    /* TODO: call readBytes multiple times in case IPMI message length
     * exceeds IPMI_MAX_MSG_LENGTH.
     */
    auto size = pollReadReady(*session, blob);
    if (size > 0)
    {
        return blob->readBytes(*session, 0, size);
    }
    return {};
}

std::vector<uint8_t> UpdateHandler::readVersion(const std::string& versionBlob)
{
    return retryIfFailed([this, versionBlob]() {
        return retryReadVersion(versionBlob);
    });
}

void UpdateHandler::cleanArtifacts()
{
    /* Errors aren't important for this call. */
    try
    {
        std::fprintf(stderr, "Executing cleanup blob\n");
        auto session =
            openBlob(blob, ipmi_flash::cleanupBlobId,
                     static_cast<std::uint16_t>(
                         ipmi_flash::FirmwareFlags::UpdateFlags::openWrite));
        blob->commit(*session, {});
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "Cleanup failed: %s\n", e.what());
    }
}

} // namespace host_tool
