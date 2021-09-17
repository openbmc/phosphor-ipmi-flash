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

#include "helper.hpp"

#include "status.hpp"
#include "tool_errors.hpp"

#include <ipmiblob/blob_errors.hpp>
#include <ipmiblob/blob_interface.hpp>

#include <chrono>
#include <thread>
#include <utility>

namespace host_tool
{

/* Poll an open verification session.  Handling closing the session is not yet
 * owned by this method.
 */
bool pollStatus(std::uint16_t session, ipmiblob::BlobInterface* blob)
{
    using namespace std::chrono_literals;

    static constexpr auto verificationSleep = 5s;
    ipmi_flash::ActionStatus result = ipmi_flash::ActionStatus::unknown;

    try
    {
        /* sleep for 5 seconds and check 360 times, for a timeout of: 1800
         * seconds (30 minutes).
         * TODO: make this command line configurable and provide smaller
         * default value.
         */
        static constexpr int commandAttempts = 360;
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

            result = static_cast<ipmi_flash::ActionStatus>(resp.metadata[0]);

            switch (result)
            {
                case ipmi_flash::ActionStatus::failed:
                    std::fprintf(stderr, "failed\n");
                    exitLoop = true;
                    break;
                case ipmi_flash::ActionStatus::unknown:
                    std::fprintf(stderr, "other\n");
                    break;
                case ipmi_flash::ActionStatus::running:
                    std::fprintf(stderr, "running\n");
                    break;
                case ipmi_flash::ActionStatus::success:
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
    return (result == ipmi_flash::ActionStatus::success);
}

/* Poll an open blob session for reading.
 *
 * The committing bit indicates that the blob is not available for reading now
 * and the reader might come back and check the state later.
 *
 * Polling finishes under the following conditions:
 * - The open_read bit set -> stat successful
 * - The open_read and committing bits unset -> stat failed;
 * - Blob exception was received;
 * - Time ran out.
 * Polling continues when the open_read bit unset and committing bit set.
 * If the blob is not open_read and not committing, then it is an error to the
 * reader.
 */
std::pair<bool, uint32_t> pollReadReady(std::uint16_t session,
                                        ipmiblob::BlobInterface* blob)
{
    using namespace std::chrono_literals;
    static constexpr auto pollingSleep = 5s;
    ipmiblob::StatResponse blobStatResp;

    try
    {
        /* Polling lasts 5 minutes. When opening a version blob, the system
         * unit defined in the version handler will extract the running version
         * from the image on the flash.
         */
        static constexpr int commandAttempts = 60;
        int attempts = 0;

        while (attempts++ < commandAttempts)
        {
            blobStatResp = blob->getStat(session);

            if (blobStatResp.blob_state & ipmiblob::StateFlags::open_read)
            {
                std::fprintf(stderr, "success\n");
                return std::make_pair(true, blobStatResp.size);
            }
            else if (blobStatResp.blob_state & ipmiblob::StateFlags::committing)
            {
                std::fprintf(stderr, "running\n");
            }
            else
            {
                std::fprintf(stderr, "failed\n");
                return std::make_pair(false, 0);
            }

            std::this_thread::sleep_for(pollingSleep);
        }
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException("blob exception received: " +
                            std::string(b.what()));
    }

    return std::make_pair(false, 0);
}

void* memcpyAligned(void* destination, const void* source, std::size_t size)
{
    std::size_t i = 0;
    std::size_t bytesCopied = 0;

    if (!(reinterpret_cast<std::uintptr_t>(destination) %
          sizeof(std::uint64_t)) &&
        !(reinterpret_cast<std::uintptr_t>(source) % sizeof(std::uint64_t)))
    {
        auto src64 = reinterpret_cast<const volatile std::uint64_t*>(source);
        auto dest64 = reinterpret_cast<volatile std::uint64_t*>(destination);

        for (i = 0; i < size / sizeof(std::uint64_t); i++)
        {
            *dest64++ = *src64++;
            bytesCopied += sizeof(std::uint64_t);
        }
    }

    auto srcMem8 =
        reinterpret_cast<const volatile std::uint8_t*>(source) + bytesCopied;
    auto destMem8 =
        reinterpret_cast<volatile std::uint8_t*>(destination) + bytesCopied;

    for (i = bytesCopied; i < size; i++)
    {
        *destMem8++ = *srcMem8++;
    }

    return destination;
}

} // namespace host_tool
