#pragma once

#include "interface.hpp"
#include "internal/sys.hpp"
#include "io.hpp"
#include "pci.hpp"
#include "progress.hpp"

#include <cstdint>
#include <ipmiblob/blob_interface.hpp>

constexpr std::uint16_t aspeedVendorId = 0x1a03;
constexpr std::uint16_t aspeedDeviceId = 0x2000;
constexpr std::size_t aspeedP2aOffset = 0x10000;
constexpr std::size_t aspeedP2aConfig = 0x0f000;
constexpr std::size_t aspeedP2aBridge = 0x0f004;
constexpr std::uint32_t p2ABridgeEnabled = 0x1;

namespace host_tool
{

class P2aDataHandler : public DataInterface
{
  public:
    P2aDataHandler(ipmiblob::BlobInterface* blob, HostIoInterface* io,
                   PciUtilInterface* pci, ProgressInterface* progress,
                   const internal::Sys* sys = &internal::sys_impl) :
        blob(blob),
        io(io), pci(pci), progress(progress), sys(sys)
    {
    }

    bool sendContents(const std::string& input, std::uint16_t session) override;
    ipmi_flash::FirmwareFlags::UpdateFlags supportedType() const override
    {
        return ipmi_flash::FirmwareFlags::UpdateFlags::p2a;
    }

  private:
    ipmiblob::BlobInterface* blob;
    HostIoInterface* io;
    PciUtilInterface* pci;
    ProgressInterface* progress;
    const internal::Sys* sys;
};

} // namespace host_tool
