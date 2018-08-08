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

#include "updatehelper.hpp"

#include <cstring>
#include <vector>

#include "config.h"

namespace
{
// Output a vector of bytes consisted of the command header and given input.
// Precondition: packet must be preallocated with kFlashCommandHdrSizeBytes.
void constructFlashIpmiPacket(int command, const uint8_t *payload,
                              int payload_size, std::vector<uint8_t> *packet)
{
    struct CommandHdr hdr;
    std::memset(&hdr, 0, sizeof(hdr));

    hdr.command = OEM_FLASH_UPDATE_BT_COMMAND;
    hdr.subcommand = command;

#ifdef ENABLE_GOOGLE
    hdr.netfn = NETFN_OEM;
    std::memcpy(&hdr.oen, OEN_GOOGLE, sizeof(OEN_GOOGLE));
#else
    hdr.netfn = NETFN_FIRMWARE;
#endif

    std::memcpy(packet->data(), &hdr, sizeof(hdr));

    if (payload != nullptr && payload_size > 0)
    {
        packet->insert(packet->end(), payload, payload + payload_size);
    }
}
} // namespace

/*
 * For use with BT Flash commands with payloads
 */
struct IpmiResponse
    IpmiUpdateHelper::SendCommand(int command,
                                  const std::vector<uint8_t> &payload)
{
    std::vector<uint8_t> packet(kFlashCommandHdrSizeBytes);
    constructFlashIpmiPacket(command, payload.data(), payload.size(), &packet);

    return raw->Raw(packet);
}

/*
 * For use with BT Flash commands that are only the command itself.
 */
struct IpmiResponse IpmiUpdateHelper::SendEmptyCommand(int command)
{
    return SendCommand(command, {});
}
