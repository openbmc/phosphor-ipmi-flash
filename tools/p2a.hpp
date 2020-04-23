#pragma once

#include "interface.hpp"
#include "internal/sys.hpp"
#include "io.hpp"
#include "pci.hpp"
#include "progress.hpp"

#include <cstdint>
#include <ipmiblob/blob_interface.hpp>
#include <vector>

constexpr std::size_t aspeedP2aConfig = 0x0f000;
constexpr std::size_t aspeedP2aBridge = 0x0f004;
constexpr std::uint32_t p2ABridgeEnabled = 0x1;

struct PciDeviceInfo
{
    std::uint16_t VID;
    std::uint16_t DID;
    std::size_t Offset;
    std::size_t Length;
    std::uint16_t bar;
};

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

    constexpr struct PciDeviceInfo static aspeedPciDeviceInfo = {
        0x1a03, 0x2000, 0x10000, 0x10000, 1};
    constexpr struct PciDeviceInfo static nuvotonPciDeviceInfo = {
        0x1050, 0x0750, 0x0, 0x4000, 0};
    const std::vector<PciDeviceInfo> PCIDeviceList = {aspeedPciDeviceInfo,
                                                      nuvotonPciDeviceInfo};
};
} // namespace host_tool
