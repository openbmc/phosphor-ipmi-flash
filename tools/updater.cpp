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

#include "firmware_handler.hpp"
#include "status.hpp"
#include "tool_errors.hpp"
#include "util.hpp"

#include <algorithm>
#include <blobs-ipmid/blobs.hpp>
#include <cstring>
#include <ipmiblob/blob_errors.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace host_tool
{

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
        std::fprintf(stderr, "Received exception '%s' on getStat\n", b.what());
        return false;
    }

    auto supported = handler->supportedType();
    if ((stat.blob_state & supported) == 0)
    {
        std::fprintf(stderr, "data interface selected not supported.\n");
        return false;
    }

    return true;
}

void UpdateHandler::sendFile(const std::string& target, const std::string& path)
{
    std::uint16_t session;
    auto supported = handler->supportedType();

    try
    {
        session = blob->openBlob(
            target, static_cast<std::uint16_t>(supported) |
                        static_cast<std::uint16_t>(blobs::OpenFlags::write));
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    if (!handler->sendContents(path, session))
    {
        /* Need to close the session on failure, or it's stuck open (until the
         * blob handler timeout is implemented, and even then, why make it wait.
         */
        blob->closeBlob(session);
        throw ToolException("Failed to send contents of " + path);
    }

    blob->closeBlob(session);
}

/* Poll an open verification session.  Handling closing the session is not yet
 * owned by this method. */
bool pollVerificationStatus(std::uint16_t session,
                            ipmiblob::BlobInterface* blob)
{
    using namespace std::chrono_literals;

    static constexpr auto verificationSleep = 5s;
    ipmi_flash::VerifyCheckResponses result =
        ipmi_flash::VerifyCheckResponses::other;

    try
    {
        static constexpr int commandAttempts = 20;
        int attempts = 0;
        bool exitLoop = false;

        /* Reach back the current status from the verification service output.
         */
        while (attempts++ < commandAttempts)
        {
            ipmiblob::StatResponse resp = blob->getStat(session);

            if (resp.metadata.size() != sizeof(std::uint8_t))
            {
                /* TODO: How do we want to handle the verification failures,
                 * because closing the session to the verify blob has a special
                 * as-of-yet not fully defined behavior.
                 */
                std::fprintf(stderr, "Received invalid metadata response!!!\n");
            }

            result =
                static_cast<ipmi_flash::VerifyCheckResponses>(resp.metadata[0]);

            switch (result)
            {
                case ipmi_flash::VerifyCheckResponses::failed:
                    std::fprintf(stderr, "failed\n");
                    exitLoop = true;
                    break;
                case ipmi_flash::VerifyCheckResponses::other:
                    std::fprintf(stderr, "other\n");
                    break;
                case ipmi_flash::VerifyCheckResponses::running:
                    std::fprintf(stderr, "running\n");
                    break;
                case ipmi_flash::VerifyCheckResponses::success:
                    std::fprintf(stderr, "success\n");
                    exitLoop = true;
                    break;
                default:
                    std::fprintf(stderr, "wat\n");
            }

            if (exitLoop)
            {
                break;
            }
            std::this_thread::sleep_for(verificationSleep);
        }
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    /* TODO: If this is reached and it's not success, it may be worth just
     * throwing a ToolException with a timeout message specifying the final
     * read's value.
     *
     * TODO: Given that excepting from certain points leaves the BMC update
     * state machine in an inconsistent state, we need to carefully evaluate
     * which exceptions from the lower layers allow one to try and delete the
     * blobs to rollback the state and progress.
     */
    return (result == ipmi_flash::VerifyCheckResponses::success);
}

bool UpdateHandler::verifyFile(const std::string& target)
{
    std::uint16_t session;
    bool success = false;

    try
    {
        session = blob->openBlob(
            target, static_cast<std::uint16_t>(blobs::OpenFlags::write));
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    std::fprintf(
        stderr,
        "Committing to verification file to trigger verification service\n");

    try
    {
        blob->commit(session, {});
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    std::fprintf(stderr,
                 "Calling stat on verification session to check status\n");

    if (pollVerificationStatus(session, blob))
    {
        std::fprintf(stderr, "Verification returned success\n");
        success = true;
    }
    else
    {
        std::fprintf(stderr, "Verification returned non-success (could still "
                             "be running (unlikely))\n");
    }

    blob->closeBlob(session);
    return (success == true);
}

void updaterMain(UpdateHandler* updater, const std::string& imagePath,
                 const std::string& signaturePath)
{
    /* TODO(venture): Add optional parameter to specify the flash type, default
     * to legacy for now.
     */
    bool goalSupported =
        updater->checkAvailable(ipmi_flash::staticLayoutBlobId);
    if (!goalSupported)
    {
        throw ToolException("Goal firmware or interface not supported");
    }

    /* Yay, our data handler is supported. */

    /* Send over the firmware image. */
    std::fprintf(stderr, "Sending over the firmware image.\n");
    updater->sendFile(ipmi_flash::staticLayoutBlobId, imagePath);

    /* Send over the hash contents. */
    std::fprintf(stderr, "Sending over the hash file.\n");
    updater->sendFile(ipmi_flash::hashBlobId, signaturePath);

    /* Trigger the verification by opening the verify file. */
    std::fprintf(stderr, "Opening the verification file\n");
    if (updater->verifyFile(ipmi_flash::verifyBlobId))
    {
        std::fprintf(stderr, "succeeded\n");
    }
    else
    {
        std::fprintf(stderr, "failed\n");
    }
}

} // namespace host_tool
