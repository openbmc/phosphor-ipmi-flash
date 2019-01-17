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

#include "pci_handler.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

namespace blobs
{

bool PciDataHandler::open()
{
    /* TODO: For the ASPEED P2A driver, this method will enable the memory
     * region to use.
     */
    return false;
}

bool PciDataHandler::close()
{
    /* TODO: Turn off the P2A bridge and region to disable host-side access.
     */
    return false;
}

std::vector<std::uint8_t> PciDataHandler::copyFrom(std::uint32_t length)
{
    /* TODO: implement this. */
    return {};
}

bool PciDataHandler::writeMeta(const std::vector<std::uint8_t>& configuration)
{
    /* PCI handler doesn't require configuration write, only read. */
    return false;
}

std::vector<std::uint8_t> PciDataHandler::readMeta()
{
    /* PCI handler does require returning a configuration from read. */
    struct PciConfigResponse reply;
    reply.address = regionAddress;

    std::vector<std::uint8_t> bytes;
    bytes.resize(sizeof(reply));
    std::memcpy(bytes.data(), &reply, sizeof(reply));

    return bytes;
}

} // namespace blobs
