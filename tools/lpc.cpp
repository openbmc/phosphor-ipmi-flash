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

#include "lpc.hpp"

#include <cstring>

namespace host_tool
{

bool LpcDataHandler::sendContents(const std::string& input,
                                  std::uint16_t session)
{
    /* TODO: Add mechanism for configuring this. */
    struct LpcRegion
    {
        /* Host LPC address at which the chunk is to be mapped. */
        std::uint32_t address;
        /* Size of the chunk to be mapped. */
        std::uint32_t length;
    };

    LpcRegion host_lpc_buf;

    /* TODO: Remove hard-coded configuration used with test machine. */
    host_lpc_buf.address = 0xfedc1000;
    host_lpc_buf.length = 0x1000;

    std::vector<std::uint8_t> payload(sizeof(host_lpc_buf));
    std::memcpy(payload.data(), &host_lpc_buf, sizeof(host_lpc_buf));

    blob->writeMeta(session, 0x00, payload);

    /* TODO: Call sessionstat and see if the metadata confirms the region was
     * mapped successfully, once the lpc data handler implements it.
     */

    /* todo:
     * configure memory region (somehow)
     * copy contents from file to memory region
     * send external chunk (writeBlob) until it's all sent.
     */
    return false;
}

} // namespace host_tool
