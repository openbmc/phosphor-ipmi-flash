#pragma once

#include "blob_interface.hpp"
#include "interface.hpp"

namespace host_tool
{

struct LpcRegion
{
    /* Host LPC address at which the chunk is to be mapped. */
    std::uint32_t address;
    /* Size of the chunk to be mapped. */
    std::uint32_t length;
};

class LpcDataHandler : public DataInterface
{
  public:
    explicit LpcDataHandler(BlobInterface* blob) : blob(blob){};

    bool sendContents(const std::string& input, std::uint16_t session) override;
    blobs::FirmwareBlobHandler::UpdateFlags supportedType() const override
    {
        return blobs::FirmwareBlobHandler::UpdateFlags::lpc;
    }

  private:
    BlobInterface* blob;
};

} // namespace host_tool
