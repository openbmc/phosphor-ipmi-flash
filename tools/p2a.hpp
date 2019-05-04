#pragma once

#include "interface.hpp"
#include "io.hpp"
#include "pci.hpp"

#include <cstdint>
#include <ipmiblob/blob_interface.hpp>

constexpr std::uint16_t aspeedVendorId = 0x1a03;
constexpr std::uint16_t aspeedDeviceId = 0x2000;
constexpr std::size_t aspeedP2aConfig = 0x0f000;
constexpr std::size_t aspeedP2aBridge = 0x0f004;
constexpr std::uint32_t p2ABridgeEnabled = 0x1;

namespace host_tool
{

class P2aDataHandler : public DataInterface
{
  public:
    P2aDataHandler(ipmiblob::BlobInterface* blob, HostIoInterface* io,
                   PciUtilInterface* pci) :
        blob(blob),
        io(io), pci(pci)
    {
    }

    bool sendContents(const std::string& input, std::uint16_t session) override;
    blobs::FirmwareBlobHandler::UpdateFlags supportedType() const override
    {
        return blobs::FirmwareBlobHandler::UpdateFlags::p2a;
    }

  private:
    ipmiblob::BlobInterface* blob;
    HostIoInterface* io;
    PciUtilInterface* pci;
};

} // namespace host_tool
