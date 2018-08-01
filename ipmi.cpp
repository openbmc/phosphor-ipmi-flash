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

#include "flash-ipmi.hpp"
#include "ipmi.hpp"

ipmi_ret_t startTransfer(UpdateInterface* updater, const uint8_t* reqBuf,
                         uint8_t* replyBuf, size_t* dataLen)
{
    /* Validate the request buffer. */
    if (sizeof(struct StartTx) > (*dataLen))
    {
        return IPMI_CC_INVALID;
    }

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
    size_t requestLength = (*dataLen);
    /* Require at least one byte. */
    if (requestLength < sizeof(struct ChunkHdr) + 1)
    {
        return IPMI_CC_INVALID;
    }

    struct ChunkHdr hdr;
    std::memcpy(&hdr, reqBuf, sizeof(hdr));

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
