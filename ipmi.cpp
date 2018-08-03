/*
 * Copyright 2017 Google Inc.
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

#include <cstring>
#include <unordered_map>

#include "flash-ipmi.hpp"
#include "ipmi.hpp"

std::unordered_map<FlashSubCmds, size_t> minimumLengths = {
    {FlashSubCmds::flashStartTransfer, sizeof(struct StartTx)},
    {FlashSubCmds::flashDataBlock, sizeof(struct ChunkHdr) + 1},
};

bool validateRequestLength(FlashSubCmds command, size_t requestLen)
{
    auto results = minimumLengths.find(command);
    if (results == minimumLengths.end())
    {
        /* Valid length by default if we don't care. */
        return true;
    }

    /* If the request is shorter than the minimum, it's invalid. */
    if (requestLen < results->second)
    {
        return false;
    }

    return true;
}

ipmi_ret_t startTransfer(UpdateInterface* updater, const uint8_t* reqBuf,
                         uint8_t* replyBuf, size_t* dataLen)
{
    auto request = reinterpret_cast<const struct StartTx*>(reqBuf);

    if (!updater->start(request->length))
    {
        return IPMI_CC_INVALID;
    }

    /* We were successful and set the response byte to 0. */
    replyBuf[0] = 0x00;
    (*dataLen) = 1;
    return IPMI_CC_OK;
}

ipmi_ret_t dataBlock(UpdateInterface* updater, const uint8_t* reqBuf,
                     uint8_t* replyBuf, size_t* dataLen)
{
    struct ChunkHdr hdr;
    std::memcpy(&hdr, reqBuf, sizeof(hdr));

    size_t requestLength = (*dataLen);

    /* Grab the bytes from the packet. */
    size_t bytesLength = requestLength - sizeof(struct ChunkHdr);
    std::vector<uint8_t> bytes(bytesLength);
    std::memcpy(bytes.data(), &reqBuf[sizeof(struct ChunkHdr)], bytesLength);

    if (!updater->flashData(hdr.offset, bytes))
    {
        return IPMI_CC_INVALID;
    }

    /* We were successful and set the response byte to 0. */
    replyBuf[0] = 0x00;
    (*dataLen) = 1;
    return IPMI_CC_OK;
}
