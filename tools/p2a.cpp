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

#include "p2a.hpp"

#include "pci.hpp"
#include "pci_handler.hpp"

#include <cstring>

namespace host_tool
{

bool P2aDataHandler::sendContents(const std::string& input,
                                  std::uint16_t session)
{
    PciDevice result;
    PciUtilImpl pci;
    PciFilter filter;
    bool found = false;

    filter.vid = aspeedVendorId;
    filter.did = aspeedDeviceId;

    /* Find the ASPEED PCI device entry we want. */
    auto output = pci.getPciDevices(filter);
    for (const auto& d : output)
    {
        std::fprintf(stderr, "[0x%x 0x%x] ", d.vid, d.did);

        /* Verify it's a memory-based bar -- we want bar1. */
        pciaddr_t bar1 = d.bars[1];
        if ((bar1 & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO)
        {
            /* We want it to not be IO-based access. */
            continue;
        }

        result = d;
        found = true;
        break;
    }

    if (!found)
    {
        return false;
    }

    std::fprintf(stderr, "\n");

    /* We sent the open command before this, so the window should be open and
     * the bridge enabled.
     */
    std::uint32_t value;
    if (!io->read(result.bars[1] | aspeedP2aConfig, sizeof(value), &value))
    {
        if (0 == (value & p2ABridgeEnabled))
        {
            std::fprintf(stderr, "Bridge not enabled.\n");
            return false;
        }
    }

    std::fprintf(stderr, "The bridge is enabled!\n");

    /* Read the configuration via blobs metadata (stat). */
    ipmiblob::StatResponse stat = blob->getStat(session);
    if (stat.metadata.size() != sizeof(blobs::PciConfigResponse))
    {
        std::fprintf(stderr, "Didn't receive expected size of metadata for "
                             "PCI Configuration response\n");
        return false;
    }

    blobs::PciConfigResponse pciResp;
    std::memcpy(&pciResp, stat.metadata.data(), sizeof(pciResp));
    std::fprintf(stderr, "Received address: 0x%x\n", pciResp.address);

#if 0
    /* Configure the mmio to point there. */
    if (!io->IoWrite(bar | kAspeedP2aBridge, sizeof(phys), &phys)) {
        // Failed to set it up, so fall back.
        std::fprintf(stderr, "Failed to update the bridge address\n");
        return false;
    }
#endif

    return false;
}

} // namespace host_tool
