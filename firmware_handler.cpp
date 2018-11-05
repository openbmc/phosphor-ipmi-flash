#include "config.h"

#include "firmware_handler.hpp"

#include <string>
#include <vector>

namespace blobs
{

std::vector<std::string> supportedFirmware = {
    "/flash/hash",
#ifdef ENABLE_STATIC_LAYOUT
    "/flash/image",
#endif
};

bool FirmwareBlobHandler::canHandleBlob(const std::string& path)
{
    return false;
}
std::vector<std::string> FirmwareBlobHandler::getBlobIds()
{
    /*
     * Grab the list of supported firmware.
     * If there's an open session, add that to this list.
     */
    std::vector<std::string> blobs = supportedFirmware;

    /*
     * If there's an open firmware session, it'll add "/flash/active/image",
     * and if the hash has started, "/flash/active/hash" regardless of
     * mechanism.
     */

    return blobs;
}
bool FirmwareBlobHandler::deleteBlob(const std::string& path)
{
    return false;
}
bool FirmwareBlobHandler::stat(const std::string& path, struct BlobMeta* meta)
{
    return false;
}
bool FirmwareBlobHandler::open(uint16_t session, uint16_t flags,
                               const std::string& path)
{
    return false;
}
std::vector<uint8_t> FirmwareBlobHandler::read(uint16_t session,
                                               uint32_t offset,
                                               uint32_t requestedSize)
{
    return {};
}
bool FirmwareBlobHandler::write(uint16_t session, uint32_t offset,
                                const std::vector<uint8_t>& data)
{
    return false;
}
bool FirmwareBlobHandler::writeMeta(uint16_t session, uint32_t offset,
                                    const std::vector<uint8_t>& data)
{
    return false;
}
bool FirmwareBlobHandler::commit(uint16_t session,
                                 const std::vector<uint8_t>& data)
{
    return false;
}
bool FirmwareBlobHandler::close(uint16_t session)
{
    return false;
}
bool FirmwareBlobHandler::stat(uint16_t session, struct BlobMeta* meta)
{
    return false;
}
bool FirmwareBlobHandler::expire(uint16_t session)
{
    return false;
}
} // namespace blobs
