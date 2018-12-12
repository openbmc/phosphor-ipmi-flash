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

#include "ipmi_errors.hpp"

#include <array>
#include <cstring>

const std::array<std::uint8_t, 3> ipmiPhosphorOen = {0xcf, 0xc2, 0x00};

std::vector<std::uint8_t>
    BlobHandler::sendIpmiPayload(const std::vector<std::uint8_t>& payload)
{
    std::vector<std::uint8_t> package;
    std::copy(ipmiPhosphorOen.begin(), ipmiPhosphorOen.end(), package.end());
    std::copy(payload.begin(), payload.end(), package.end());

    try
    {
        auto fullresp = ipmi->sendPacket(package);
    }
    catch (const IpmiException& e)
    {
        std::fprintf(stderr, "Received exception: %s\n", e.what());
    }

    /* TODO(venture): strip out the OEN, and check the CRC */
    return {};
}

int BlobHandler::getBlobCount()
{
    std::uint32_t count;
    auto resp = sendIpmiPayload({BlobOEMCommands::bmcBlobGetCount});
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
