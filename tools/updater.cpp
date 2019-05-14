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

#include "updater.hpp"

#include "tool_errors.hpp"

#include <algorithm>
#include <blobs-ipmid/blobs.hpp>
#include <cstring>
#include <ipmiblob/blob_errors.hpp>
#include <memory>
#include <string>

namespace host_tool
{

void updaterMain(ipmiblob::BlobInterface* blob, DataInterface* handler,
                 const std::string& imagePath, const std::string& signaturePath)
{
    /* TODO(venture): Add optional parameter to specify the flash type, default
     * to legacy for now.
     */
    std::string goalFirmware = "/flash/image";
    std::string hashFilename = "/flash/hash";

    /* Get list of blob_ids, check for /flash/image, or /flash/tarball.
     * TODO(venture) the mechanism doesn't care, but the caller of burn_my_bmc
     * will have in mind which they're sending and we need to verify it's
     * available and use it.
     */
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
        throw ToolException(goalFirmware + " not found");
    }

    /* Call stat on /flash/image (or /flash/tarball) and check if data interface
     * is supported.
     */
    ipmiblob::StatResponse stat;
    try
    {
        stat = blob->getStat(goalFirmware);
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    auto supported = handler->supportedType();
    if ((stat.blob_state & supported) == 0)
    {
        throw ToolException("data interface selected not supported.");
    }

    /* Yay, our data handler is supported. */

    /* Send over the firmware image. */
    std::fprintf(stderr, "Sending over the firmware image.\n");
    std::uint16_t session;
    try
    {
        session = blob->openBlob(
            goalFirmware,
            static_cast<std::uint16_t>(supported) |
                static_cast<std::uint16_t>(blobs::OpenFlags::write));
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    if (!handler->sendContents(imagePath, session))
    {
        /* Need to close the session on failure, or it's stuck open (until the
         * blob handler timeout is implemented, and even then, why make it wait.
         */
        blob->closeBlob(session);
        throw ToolException("Failed to send contents of " + imagePath);
    }

    blob->closeBlob(session);

    /* Send over the hash contents. */
    std::fprintf(stderr, "Sending over the hash file.\n");
    try
    {
        session = blob->openBlob(
            hashFilename,
            static_cast<std::uint16_t>(supported) |
                static_cast<std::uint16_t>(blobs::OpenFlags::write));
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    if (!handler->sendContents(signaturePath, session))
    {
        blob->closeBlob(session);
        throw ToolException("Failed to send contents of " + signaturePath);
    }

    blob->closeBlob(session);

    /* Trigger the verification. */
    /* Check the verification. */

    return;
}

} // namespace host_tool
