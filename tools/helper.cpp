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

#include <algorithm>
#include <chrono>
#include <optional>
#include <thread>
#include <utility>

namespace host_tool
{

template <typename Check>
static auto pollStat(std::uint16_t session, ipmiblob::BlobInterface* blob,
                     Check&& check)
{
    using namespace std::chrono_literals;

    constexpr auto maxSleep = 1s;
    constexpr auto printInterval = 30s;
    constexpr auto timeout = 30min;

    try
    {
        auto start = std::chrono::steady_clock::now();
        auto last_print = start;
        auto last_check = start;
        auto check_interval = 50ms;

        while (true)
        {
            ipmiblob::StatResponse resp = blob->getStat(session);
            auto ret = check(resp);
            if (ret.has_value())
            {
                std::fprintf(stderr, "success\n");
                return std::move(*ret);
            }

            auto cur = std::chrono::steady_clock::now();
            if (cur - last_print >= printInterval)
            {
                std::fprintf(stderr, "running\n");
                last_print = cur;
            }

            auto sleep = check_interval - (cur - last_check);
            last_check = cur;
            // Check that we don't timeout immediately after sleeping
            // to avoid an extra sleep without checking
            if (cur - start > timeout - sleep)
            {
                throw ToolException("Stat check timed out");
            }
            check_interval = std::min<decltype(check_interval)>(
                check_interval * 2, maxSleep);
            std::this_thread::sleep_for(sleep);
        }
    }
    catch (const ipmiblob::BlobException& b)
    {
        throw ToolException(
            "blob exception received: " + std::string(b.what()));
    }
}

/* Poll an open verification session.  Handling closing the session is not yet
 * owned by this method.
 */
void pollStatus(std::uint16_t session, ipmiblob::BlobInterface* blob)
{
    pollStat(session, blob,
             [](const ipmiblob::StatResponse& resp) -> std::optional<bool> {
                 if (resp.metadata.size() != 1)
                 {
                     throw ToolException("Invalid stat metadata");
                 }
                 auto result =
                     static_cast<ipmi_flash::ActionStatus>(resp.metadata[0]);
                 switch (result)
                 {
                     case ipmi_flash::ActionStatus::failed:
                         throw ToolException("BMC reported failure");
                     case ipmi_flash::ActionStatus::unknown:
                     case ipmi_flash::ActionStatus::running:
                         return std::nullopt;
                     case ipmi_flash::ActionStatus::success:
                         return true;
                     default:
                         throw ToolException("Unrecognized action status");
                 }
             });
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
uint32_t pollReadReady(std::uint16_t session, ipmiblob::BlobInterface* blob)
{
    return pollStat(
        session, blob,
        [](const ipmiblob::StatResponse& resp) -> std::optional<uint32_t> {
            if (resp.blob_state & ipmiblob::StateFlags::open_read)
            {
                return resp.size;
            }
            if (resp.blob_state & ipmiblob::StateFlags::committing)
            {
                return std::nullopt;
            }
            throw ToolException("BMC blob failed to become ready");
        });
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

    auto srcMem8 = reinterpret_cast<const volatile std::uint8_t*>(source) +
                   bytesCopied;
    auto destMem8 = reinterpret_cast<volatile std::uint8_t*>(destination) +
                    bytesCopied;

    for (i = bytesCopied; i < size; i++)
    {
        *destMem8++ = *srcMem8++;
    }

    return destination;
}

} // namespace host_tool
