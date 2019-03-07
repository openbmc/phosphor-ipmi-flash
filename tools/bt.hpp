#pragma once

#include "interface.hpp"
#include "internal/sys.hpp"

#include <ipmiblob/blob_interface.hpp>

namespace host_tool
{

class BtDataHandler : public DataInterface
{
  public:
    BtDataHandler(ipmiblob::BlobInterface* blob,
                  const internal::Sys* sys = &internal::sys_impl) :
        blob(blob),
        sys(sys){};

    bool sendContents(const std::string& input, std::uint16_t session) override;
    blobs::FirmwareBlobHandler::UpdateFlags supportedType() const override
    {
        return blobs::FirmwareBlobHandler::UpdateFlags::ipmi;
    }

  private:
    ipmiblob::BlobInterface* blob;
    const internal::Sys* sys;
};

} // namespace host_tool
