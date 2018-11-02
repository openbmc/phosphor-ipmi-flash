#include "firmware_handler.hpp"

namespace blobs
{

bool FirmwareBlobHandler::canHandleBlob(const std::string& path)
{
    return false;
}
std::vector<std::string> FirmwareBlobHandler::getBlobIds()
{
    return {};
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
