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

#include "firmware_handler.hpp"
#include "pci.hpp"
#include "pci_handler.hpp"

#include <cstring>
#include <ipmiblob/blob_errors.hpp>

namespace host_tool
{

bool P2aDataHandler::sendContents(const std::string& input,
                                  std::uint16_t session)
{
    PciDevice result;
    PciUtilImpl pci;
    PciFilter filter;
    bool found = false;
    pciaddr_t bar1;

    filter.vid = aspeedVendorId;
    filter.did = aspeedDeviceId;

    /* Find the ASPEED PCI device entry we want. */
    auto output = pci.getPciDevices(filter);
    for (const auto& d : output)
    {
        std::fprintf(stderr, "[0x%x 0x%x] ", d.vid, d.did);

        /* Verify it's a memory-based bar -- we want bar1. */
        bar1 = d.bars[1];
        if ((bar1 & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO)
        {
            /* We want it to not be IO-based access. */
            continue;
        }

        /* For now capture the entire device even if we're only using BAR1 */
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
    if (!io->read(bar1 | aspeedP2aConfig, sizeof(value), &value))
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

    /* Configure the mmio to point there. */
    if (!io->write(bar1 | aspeedP2aBridge, sizeof(pciResp.address),
                   &pciResp.address))
    {
        // Failed to set it up, so fall back.
        std::fprintf(stderr, "Failed to update the bridge address\n");
        return false;
    }

    /* For data blocks in 64kb, stage data, and send blob write command. */
    int inputFd = sys->open(input.c_str(), 0);
    if (inputFd < 0)
    {
        return false;
    }

    int bytesRead = 0;
    std::uint32_t offset = 0;
    const std::uint32_t p2aLength = aspeedP2aOffset;

    auto readBuffer = std::make_unique<std::uint8_t[]>(p2aLength);
    if (nullptr == readBuffer)
    {
        std::fprintf(stderr, "Unable to allocate memory for read buffer.\n");
        return false;
    }

    try
    {
        do
        {
            bytesRead = sys->read(inputFd, readBuffer.get(), p2aLength);
            if (bytesRead > 0)
            {
                std::fprintf(stderr, "bytesRead: %d\n", bytesRead);

                /* TODO: Will likely need to store an rv somewhere to know when
                 * we're exiting from failure.
                 */
                if (!io->write(bar1 | aspeedP2aOffset, bytesRead,
                               readBuffer.get()))
                {
                    std::fprintf(stderr,
                                 "Failed to write to region in memory!\n");
                    break;
                }

                /* Ok, so the data is staged, now send the blob write with the
                 * details.
                 */
                struct blobs::ExtChunkHdr chunk;
                chunk.length = bytesRead;
                std::vector<std::uint8_t> chunkBytes(sizeof(chunk));
                std::memcpy(chunkBytes.data(), &chunk, sizeof(chunk));

                /* This doesn't return anything on success. */
                blob->writeBytes(session, offset, chunkBytes);
                offset += bytesRead;
            }
        } while (bytesRead > 0);
    }
    catch (const ipmiblob::BlobException& b)
    {
        sys->close(inputFd);
        return false;
    }

    sys->close(inputFd);
    return true;
}

} // namespace host_tool
