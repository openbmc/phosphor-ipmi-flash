#pragma once

#include "blob_interface.hpp"
#include "interface.hpp"

namespace host_tool
{

class LpcDataHandler : public DataInterface
{
  public:
    explicit LpcDataHandler(BlobInterface* blob) : blob(blob){};

    bool sendContents(const std::string& input, std::uint16_t session) override;
    blobs::FirmwareBlobHandler::UpdateFlags supportedType() const override
    {
        return flags;
    }

  private:
    BlobInterface* blob;
    blobs::FirmwareBlobHandler::UpdateFlags flags =
        blobs::FirmwareBlobHandler::UpdateFlags::lpc;
};

} // namespace host_tool
