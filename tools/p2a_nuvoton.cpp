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


#include "data.hpp"
#include "flags.hpp"
#include "p2a.hpp"
#include "pci.hpp"

#include <cstdint>
#include <cstring>
#include <ipmiblob/blob_errors.hpp>
#include <memory>
#include <string>

namespace host_tool
{

bool P2aDataHandler::sendContents(const std::string& input,
                                  std::uint16_t session)
{
    PciDevice result;
    PciFilter filter;
    bool found = false;
    pciaddr_t bar0;

#ifdef NUVOTON_P2A_VGA
    filter.vid = nuvotonVGAVendorId;
    filter.did = nuvotonVGADeviceId;
#else
    filter.vid = nuvotonMBOXVendorId;
    filter.did = nuvotonMBOXDeviceId;
#endif

    /* Find the NUVOTON PCI device entry we want. */
    auto output = pci->getPciDevices(filter);
    for (const auto& d : output)
    {
        std::fprintf(stderr, "[0x%x 0x%x] ", d.vid, d.did);

        /* Verify it's a memory-based bar -- we want bar0. */
        bar0 = d.bars[0];
        std::fprintf(stderr, "bar0[0x%x] ", (unsigned int)bar0);
        if ((bar0 & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO)
        {
            /* We want it to not be IO-based access. */
            continue;
        }
        bar0 &= 0xFFFFFFF0;
        /* For now capture the entire device even if we're only using BAR0 */
        result = d;
        found = true;
        break;
    }

    if (!found)
    {
        return false;
    }

    std::fprintf(stderr, "\n");

    std::fprintf(stderr, "The bridge is enabled!\n");

    /* Read the configuration via blobs metadata (stat). */
    ipmiblob::StatResponse stat = blob->getStat(session);
    if (stat.metadata.size() != sizeof(ipmi_flash::PciConfigResponse))
    {
        std::fprintf(stderr, "Didn't receive expected size of metadata for "
                             "PCI Configuration response\n");
        return false;
    }

    ipmi_flash::PciConfigResponse pciResp;
    std::memcpy(&pciResp, stat.metadata.data(), sizeof(pciResp));
    std::fprintf(stderr, "Received address: 0x%x\n", pciResp.address);

    /* For data blocks in 64kb, stage data, and send blob write command. */
    int inputFd = sys->open(input.c_str(), 0);
    if (inputFd < 0)
    {
        return false;
    }

    std::int64_t fileSize = sys->getSize(input.c_str());
    if (fileSize == 0)
    {
        std::fprintf(stderr, "Zero-length file, or other file access error\n");
        return false;
    }
    else
        std::fprintf(stderr, "file [%s] size is %ld\n", input.c_str(),
                     fileSize);

    progress->start(fileSize);

    std::uint32_t p2aLength = 0x4000;

    auto readBuffer = std::make_unique<std::uint8_t[]>(p2aLength);
    if (nullptr == readBuffer)
    {
        std::fprintf(stderr, "Unable to allocate memory for read buffer.\n");
        return false;
    }

    try
    {
        int bytesRead = 0;
        std::uint32_t offset = 0;

#ifdef NUVOTON_P2A_VGA
        std::uint32_t P2aOffset = 0x400000;
#else
        std::uint32_t P2aOffset = 0;
#endif

        do
        {
            bytesRead = sys->read(inputFd, readBuffer.get(), p2aLength);
            if (bytesRead > 0)
            {
                /* TODO: Will likely need to store an rv somewhere to know when
                 * we're exiting from failure.
                 */
                if (!io->write(bar0 | P2aOffset, bytesRead, readBuffer.get()))
                {
                    std::fprintf(stderr,
                                 "Failed to write to region in memory!\n");
                    break;
                }

                /* Ok, so the data is staged, now send the blob write with the
                 * details.
                 */
                struct ipmi_flash::ExtChunkHdr chunk;
                chunk.length = bytesRead;
                std::vector<std::uint8_t> chunkBytes(sizeof(chunk));
                std::memcpy(chunkBytes.data(), &chunk, sizeof(chunk));
                /* This doesn't return anything on success. */
                blob->writeBytes(session, offset, chunkBytes);
                offset += bytesRead;
                progress->updateProgress(bytesRead);
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
