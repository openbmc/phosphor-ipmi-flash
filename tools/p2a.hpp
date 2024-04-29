#pragma once

#include "interface.hpp"
#include "internal/sys.hpp"
#include "pci.hpp"
#include "progress.hpp"

#include <ipmiblob/blob_interface.hpp>

#include <cstdint>
#include <vector>

namespace host_tool
{

class P2aDataHandler : public DataInterface
{
  public:
    explicit P2aDataHandler(ipmiblob::BlobInterface* blob, const PciAccess* pci,
                            ProgressInterface* progress, bool skipBridgeDisable,
                            const internal::Sys* sys = &internal::sys_impl) :
        blob(blob), pci(pci), progress(progress),
        skipBridgeDisable(skipBridgeDisable), sys(sys)
    {}

    P2aDataHandler(ipmiblob::BlobInterface* blob, const PciAccess* pci,
                   ProgressInterface* progress,
                   const internal::Sys* sys = &internal::sys_impl) :
        P2aDataHandler(blob, pci, progress, false, sys)
    {}

    bool sendContents(const std::string& input, std::uint16_t session) override;
    ipmi_flash::FirmwareFlags::UpdateFlags supportedType() const override
    {
        return ipmi_flash::FirmwareFlags::UpdateFlags::p2a;
    }

  private:
    ipmiblob::BlobInterface* blob;
    const PciAccess* pci;
    ProgressInterface* progress;
    bool skipBridgeDisable;
    const internal::Sys* sys;
};

} // namespace host_tool
