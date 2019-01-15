#pragma once

#include "blob_interface.hpp"
#include "interface.hpp"
#include "internal/sys.hpp"

namespace host_tool
{

class BtDataHandler : public DataInterface
{
  public:
    BtDataHandler(BlobInterface* blob,
                  const internal::Sys* sys = &internal::sys_impl) :
        blob(blob),
        sys(sys){};

    bool sendContents(const std::string& input, std::uint16_t session) override;
    blobs::FirmwareBlobHandler::UpdateFlags supportedType() const override
    {
        return flags;
    }

  private:
    BlobInterface* blob;
    const internal::Sys* sys;
    blobs::FirmwareBlobHandler::UpdateFlags flags =
        blobs::FirmwareBlobHandler::UpdateFlags::ipmi;
};

} // namespace host_tool
