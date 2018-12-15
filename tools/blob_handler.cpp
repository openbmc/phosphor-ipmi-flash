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

#include "blob_errors.hpp"
#include "crc.hpp"
#include "ipmi_errors.hpp"

#include <array>
#include <cstring>

namespace host_tool
{

namespace
{
const std::array<std::uint8_t, 3> ipmiPhosphorOen = {0xcf, 0xc2, 0x00};
}

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

        std::copy(src, src + sizeof(crc), std::back_inserter(request));

        /* Copy the payload. */
        std::copy(payload.begin(), payload.end(), std::back_inserter(request));
    }

    try
    {
        reply = ipmi->sendPacket(request);
    }
    catch (const IpmiException& e)
    {
        throw BlobException(e.what());
    }

    /* IPMI_CC was OK, and it returned no bytes, so let's be happy with that for
     * now.
     */
    if (reply.size() == 0)
    {
        return reply;
    }

    std::size_t headerSize = ipmiPhosphorOen.size() + sizeof(std::uint16_t);

    /* This cannot be a response because it's smaller than the smallest
     * response.
     */
    if (reply.size() < headerSize)
    {
        throw BlobException("Invalid response length");
    }

    /* Verify the OEN. */
    if (std::memcmp(ipmiPhosphorOen.data(), reply.data(),
                    ipmiPhosphorOen.size()) != 0)
    {
        throw BlobException("Invalid OEN received");
    }

    /* Validate CRC. */
    std::uint16_t crc;
    auto ptr = reinterpret_cast<std::uint8_t*>(&crc);
    std::memcpy(ptr, &reply[ipmiPhosphorOen.size()], sizeof(crc));

    for (const auto& byte : reply)
    {
        std::fprintf(stderr, "0x%02x ", byte);
    }
    std::fprintf(stderr, "\n");

    std::fprintf(stderr, "replysize: %d\n", static_cast<int>(reply.size()));
    std::vector<std::uint8_t> bytes;
    bytes.insert(bytes.begin(), reply.begin() + headerSize, reply.end());

    for (const auto& byte : bytes)
    {
        std::fprintf(stderr, "0x%02x ", byte);
    }
    std::fprintf(stderr, "\n");

    auto computed = generateCrc(bytes);
    if (crc != computed)
    {
        std::fprintf(stderr, "Invalid CRC, received: 0x%x, computed: 0x%x\n",
                     crc, computed);
        throw BlobException("Invalid CRC on received data.");
    }

    return bytes;
}

int BlobHandler::getBlobCount()
{
    std::uint32_t count;
    try
    {
        auto resp = sendIpmiPayload(BlobOEMCommands::bmcBlobGetCount, {});
        if (resp.size() != sizeof(count))
        {
            return 0;
        }

        /* LE to LE (need to make this portable as some point. */
        std::memcpy(&count, resp.data(), sizeof(count));
    }
    catch (const BlobException& b)
    {
        return 0;
    }

    std::fprintf(stderr, "BLOB Count: %d\n", count);
    return count;
}

std::string BlobHandler::enumerateBlob(std::uint32_t index)
{
    std::vector<std::uint8_t> payload;
    std::string output;
    std::uint8_t* data = reinterpret_cast<std::uint8_t*>(&index);

    std::copy(data, data + sizeof(std::uint32_t), std::back_inserter(payload));

    try
    {
        auto resp = sendIpmiPayload(BlobOEMCommands::bmcBlobEnumerate, payload);
        std::copy(resp.begin(), resp.end(), std::back_inserter(output));
    }
    catch (const BlobException& b)
    {
        return "";
    }

    std::fprintf(stderr, "blobid: '%s'\n", output.c_str());
    return output;
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

StatResponse BlobHandler::getStat(const std::string& id)
{
    StatResponse meta;
    std::vector<std::uint8_t> name, resp;

    std::copy(id.begin(), id.end(), std::back_inserter(name));
    name.push_back(0x00); /* need to add nul-terminator. */

    try
    {
        resp = sendIpmiPayload(BlobOEMCommands::bmcBlobStat, name);
    }
    catch (const BlobException& b)
    {
        throw;
    }

    std::memcpy(&meta.blob_state, &resp[0], sizeof(meta.blob_state));
    std::memcpy(&meta.size, &resp[sizeof(meta.blob_state)], sizeof(meta.size));
    int offset = sizeof(meta.blob_state) + sizeof(meta.size);
    std::uint8_t len = resp[offset];
    if (len > 0)
    {
        std::copy(&resp[offset + 1], &resp[resp.size()],
                  std::back_inserter(meta.metadata));
    }

    return meta;
}

std::uint16_t
    BlobHandler::openBlob(const std::string& id,
                          blobs::FirmwareBlobHandler::UpdateFlags handlerFlags)
{
    std::uint16_t session;
    std::vector<std::uint8_t> request, resp;
    std::uint16_t flags =
        blobs::FirmwareBlobHandler::UpdateFlags::openWrite | handlerFlags;
    auto addrFlags = reinterpret_cast<std::uint8_t*>(&flags);

    std::copy(addrFlags, addrFlags + sizeof(flags),
              std::back_inserter(request));
    std::copy(id.begin(), id.end(), std::back_inserter(request));
    request.push_back(0x00); /* need to add nul-terminator. */

    try
    {
        resp = sendIpmiPayload(BlobOEMCommands::bmcBlobOpen, request);
    }
    catch (const BlobException& b)
    {
        throw;
    }

    if (resp.size() != sizeof(session))
    {
        throw BlobException("Did not receive session.");
    }

    std::memcpy(&session, resp.data(), sizeof(session));
    return session;
}

void BlobHandler::closeBlob(std::uint16_t session)
{
    std::vector<std::uint8_t> request, resp;
    auto addrSession = reinterpret_cast<std::uint8_t*>(&session);
    std::copy(addrSession, addrSession + sizeof(session),
              std::back_inserter(request));

    try
    {
        resp = sendIpmiPayload(BlobOEMCommands::bmcBlobClose, request);
    }
    catch (const BlobException& b)
    {
        std::fprintf(stderr, "Received failure on close: %s\n", b.what());
    }

    return;
}

} // namespace host_tool
