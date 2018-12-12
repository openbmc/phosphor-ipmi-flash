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

#include "blob_handler.hpp"

#include "crc.hpp"
#include "ipmi_errors.hpp"

#include <array>
#include <cstring>

const std::array<std::uint8_t, 3> ipmiPhosphorOen = {0xcf, 0xc2, 0x00};

std::vector<std::uint8_t>
    BlobHandler::sendIpmiPayload(BlobOEMCommands command,
                                 const std::vector<std::uint8_t>& payload)
{
    std::vector<std::uint8_t> request, reply;

    std::copy(ipmiPhosphorOen.begin(), ipmiPhosphorOen.end(),
              std::back_inserter(request));
    request.push_back(command);

    if (payload.size() > 0)
    {
        /* Grow the vector to hold the bytes. */
        request.reserve(request.size() + sizeof(std::uint16_t));

        /* CRC required. */
        std::uint16_t crc = generateCrc(payload);
        std::uint8_t* src = reinterpret_cast<std::uint8_t*>(&crc);

        std::copy(src, src + 2, std::back_inserter(request));

        /* Copy the payload. */
        std::copy(payload.begin(), payload.end(), std::back_inserter(request));
    }

    try
    {
        reply = ipmi->sendPacket(request);
    }
    catch (const IpmiException& e)
    {
        std::fprintf(stderr, "Received exception: %s\n", e.what());
        return {};
    }

    /* IPMI_CC was OK, and it returned no bytes, so let's be happy with that for
     * now.
     */
    if (reply.size() == 0)
    {
        return reply;
    }

    size_t headerSize = ipmiPhosphorOen.size() + sizeof(std::uint16_t);

    /* This cannot be a response because it's smaller than the smallest
     * response.
     */
    if (reply.size() < headerSize)
    {
        std::fprintf(stderr, "Invalid response length\n");
        return {};
    }

    /* Verify the OEN. */
    if (std::memcmp(ipmiPhosphorOen.data(), reply.data(),
                    ipmiPhosphorOen.size()) != 0)
    {
        std::fprintf(stderr, "Invalid OEN received\n");
        return {};
    }

    /* TODO(venture): Validate CRC. */
    std::vector<std::uint8_t> bytes;
    std::copy(&reply[headerSize], &reply[reply.size()],
              std::back_inserter(bytes));
    return bytes;
}

int BlobHandler::getBlobCount()
{
    std::uint32_t count;
    auto resp = sendIpmiPayload(BlobOEMCommands::bmcBlobGetCount, {});
    if (resp.size() != sizeof(count))
    {
        return 0;
    }

    /* LE to LE (need to make this portable as some point. */
    std::memcpy(&count, resp.data(), sizeof(count));
    return count;
}

std::string BlobHandler::enumerateBlob(int index)
{
    return "";
}

std::vector<std::string> BlobHandler::getBlobList()
{
    std::vector<std::string> list;
    int blobCount = getBlobCount();

    for (int i = 0; i < blobCount; i++)
    {
        auto name = enumerateBlob(i);
        /* Currently ignore failures. */
        if (!name.empty())
        {
            list.push_back(name);
        }
    }

    return list;
}
