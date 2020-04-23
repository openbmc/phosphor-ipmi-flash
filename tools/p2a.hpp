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

/* We support below two PCI devices now,
 * burn_my_bmc will scan existing PCI device,
 * when interface is ipmipci .
 */
constexpr std::uint16_t aspeedVendorId = 0x1a03;
constexpr std::uint16_t aspeedDeviceId = 0x2000;
constexpr std::size_t aspeedP2aOffset = 0x10000;
constexpr std::size_t aspeedp2aLength = 0x10000;

constexpr std::uint16_t nuvotonVendorId = 0x1050;
constexpr std::uint16_t nuvotonDeviceId = 0x0750;
constexpr std::size_t nuvotonP2aOffset = 0;
constexpr std::uint32_t nuvotonp2aLength = 0x4000;

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

    std::vector<std::vector<std::uint32_t>> PCIDeviceList = {
        {aspeedVendorId, aspeedDeviceId, aspeedP2aOffset, aspeedP2aOffset, 1},
        {nuvotonVendorId, nuvotonDeviceId, nuvotonP2aOffset, nuvotonp2aLength,
         0}};
};
} // namespace host_tool
