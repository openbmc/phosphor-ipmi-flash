#pragma once

#include "blob_interface.hpp"
#include "interface.hpp"
#include "internal/sys.hpp"

#include <cstdint>

namespace host_tool
{

struct LpcRegion
{
    /* Host LPC address at which the chunk is to be mapped. */
    std::uint32_t address;
    /* Size of the chunk to be mapped. */
    std::uint32_t length;
} __attribute__((packed));

class LpcDataHandler : public DataInterface
{
  public:
    LpcDataHandler(BlobInterface* blob,
                   const internal::Sys* sys = &internal::sys_impl) :
        blob(blob),
        sys(sys){};

    bool sendContents(const std::string& input, std::uint16_t session) override;
    blobs::FirmwareBlobHandler::UpdateFlags supportedType() const override
    {
        return blobs::FirmwareBlobHandler::UpdateFlags::lpc;
    }

  private:
    BlobInterface* blob;
    const internal::Sys* sys;
};

} // namespace host_tool
