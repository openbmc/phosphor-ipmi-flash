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

#include "data.hpp"
#include "flags.hpp"
#include "pci.hpp"

#include <cstdint>
#include <cstring>
#include <ipmiblob/blob_errors.hpp>
#include <memory>
#include <string>

namespace host_tool
{

namespace
{
void disablePciBridge(HostIoInterface* io, std::size_t address)
{
    /* Read current value, and just blindly unset the bit. */
    std::uint32_t value;
    if (!io->read(address + aspeedP2aConfig, sizeof(value), &value))
    {
        return;
    }

    value &= ~p2ABridgeEnabled;
    io->write(address + aspeedP2aConfig, sizeof(value), &value);
}

} // namespace

bool P2aDataHandler::sendContents(const std::string& input,
                                  std::uint16_t session)
{
    PciDevice result;
    PciFilter filter;
    bool found = false;
    pciaddr_t bar;
    bool returnValue = false;
    int inputFd = -1;
    ipmi_flash::PciConfigResponse pciResp;
    std::int64_t fileSize;
    std::unique_ptr<std::uint8_t[]> readBuffer;

    std::uint16_t pciDeviceVID;
    std::uint16_t pciDeviceDID;
    std::uint32_t p2aOffset;
    std::uint32_t p2aLength;

    for (auto device : PCIDeviceList)
    {
        filter.vid = device.VID;
        filter.did = device.DID;

        /* Find the PCI device entry we want. */
        auto output = pci->getPciDevices(filter);
        for (const auto& d : output)
        {
            std::fprintf(stderr, "[0x%x 0x%x] \n", d.vid, d.did);

            /* Verify it's a memory-based bar. */
            bar = d.bars[device.bar];

            if ((bar & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO)
            {
                /* We want it to not be IO-based access. */
                continue;
            }

            /* For now capture the entire device even if we're only using BAR0
             */
            result = d;
            found = true;
            break;
        }

        if (found)
        {
            std::fprintf(stderr, "Find [0x%x 0x%x] \n", device.VID, device.DID);
            std::fprintf(stderr, "bar%u[0x%x] \n", device.bar,
                         (unsigned int)bar);
            pciDeviceVID = device.VID;
            pciDeviceDID = device.DID;
            p2aOffset = device.Offset;
            p2aLength = device.Length;
            break;
        }
    }

    if (!found)
    {
        return false;
    }

    std::fprintf(stderr, "\n");

    /* We sent the open command before this, so the window should be open and
     * the bridge enabled on the BMC.
     */
    if (pciDeviceVID == aspeedPciDeviceInfo.VID &&
        pciDeviceDID == aspeedPciDeviceInfo.DID)
    {
        std::uint32_t value;
        if (!io->read(bar + aspeedP2aConfig, sizeof(value), &value))
        {
            std::fprintf(stderr, "PCI config read failed\n");
            return false;
        }

        if (0 == (value & p2ABridgeEnabled))
        {
            std::fprintf(stderr, "Bridge not enabled - Enabling from host\n");

            value |= p2ABridgeEnabled;
            if (!io->write(bar + aspeedP2aConfig, sizeof(value), &value))
            {
                std::fprintf(stderr, "PCI config write failed\n");
                return false;
            }
        }

        /* From this point down we need to disable the bridge. */
        std::fprintf(stderr, "The bridge is enabled!\n");
    }

    /* Read the configuration via blobs metadata (stat). */
    ipmiblob::StatResponse stat = blob->getStat(session);
    if (stat.metadata.size() != sizeof(ipmi_flash::PciConfigResponse))
    {
        std::fprintf(stderr, "Didn't receive expected size of metadata for "
                             "PCI Configuration response\n");
        goto exit;
    }

    std::memcpy(&pciResp, stat.metadata.data(), sizeof(pciResp));
    std::fprintf(stderr, "Received address: 0x%x\n", pciResp.address);

    if (pciDeviceVID == aspeedPciDeviceInfo.VID &&
        pciDeviceDID == aspeedPciDeviceInfo.DID)
    {
        /* Configure the mmio to point there. */
        if (!io->write(bar + aspeedP2aBridge, sizeof(pciResp.address),
                       &pciResp.address))
        {
            // Failed to set it up, so fall back.
            std::fprintf(stderr, "Failed to update the bridge address\n");
            goto exit;
        }
    }

    /* For data blocks in 64kb, stage data, and send blob write command. */
    inputFd = sys->open(input.c_str(), 0);
    if (inputFd < 0)
    {
        std::fprintf(stderr, "Unable to open file: '%s'\n", input.c_str());
        goto exit;
    }

    fileSize = sys->getSize(input.c_str());
    if (fileSize == 0)
    {
        std::fprintf(stderr, "Zero-length file, or other file access error\n");
        goto exit;
    }

    progress->start(fileSize);

    readBuffer = std::make_unique<std::uint8_t[]>(p2aLength);
    if (nullptr == readBuffer)
    {
        std::fprintf(stderr, "Unable to allocate memory for read buffer.\n");
        goto exit;
    }

    try
    {
        int bytesRead = 0;
        std::uint32_t offset = 0;

        do
        {
            bytesRead = sys->read(inputFd, readBuffer.get(), p2aLength);
            if (bytesRead > 0)
            {
                /* TODO: Will likely need to store an rv somewhere to know when
                 * we're exiting from failure.
                 */
                if (!io->write(bar + p2aOffset, bytesRead, readBuffer.get()))
                {
                    std::fprintf(stderr,
                                 "Failed to write to region in memory!\n");
                    goto exit;
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
        goto exit;
    }

    /* defaults to failure. */
    returnValue = true;

exit:
    /* disable ASPEED PCI bridge. */
    if (pciDeviceVID == aspeedPciDeviceInfo.VID &&
        pciDeviceDID == aspeedPciDeviceInfo.DID)
    {
        disablePciBridge(io, bar);
    }

    /* close input file. */
    if (inputFd != -1)
    {
        sys->close(inputFd);
    }
    return returnValue;
}

} // namespace host_tool
