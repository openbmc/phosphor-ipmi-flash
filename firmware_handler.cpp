#include "firmware_handler.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

const std::string FirmwareBlobHandler::hashBlobID = "/flash/hash";

std::unique_ptr<GenericBlobInterface>
    FirmwareBlobHandler::CreateFirmwareBlobHandler(
        const std::vector<std::string>& firmwares, std::uint32_t transports)
{
    std::vector<std::string> blobs = firmwares;
    blobs.push_back(hashBlobID);

    return std::make_unique<FirmwareBlobHandler>(blobs, transports);
}

bool FirmwareBlobHandler::canHandleBlob(const std::string& path)
{
    /* Check if the path is in our supported list (or active list). */
    if (std::count(blobIDs.begin(), blobIDs.end(), path))
    {
        return true;
    }

    return false;
}

std::vector<std::string> FirmwareBlobHandler::getBlobIds()
{
    /*
     * Grab the list of supported firmware.
     *
     * If there's an open firmware session, it'll already be present in the
     * list as "/flash/active/image", and if the hash has started,
     * "/flash/active/hash" regardless of mechanism.  This is done in the open
     * comamnd, no extra work is required here.
     */
    return blobIDs;
}

bool FirmwareBlobHandler::deleteBlob(const std::string& path)
{
    /*
     * Per the design, this mean abort, and this will trigger whatever
     * appropriate actions are required to abort the process.
     */
    return false;
}

bool FirmwareBlobHandler::stat(const std::string& path, struct BlobMeta* meta)
{
    /*
     * Stat on the files will return information such as what supported
     * transport mechanisms are available.
     *
     * Stat on an active file or hash will return information such as the size
     * of the data cached, and any additional pertinent information.  The
     * blob_state on the active files will return the state of the update.
     */
    return false;
}

bool FirmwareBlobHandler::open(uint16_t session, uint16_t flags,
                               const std::string& path)
{
    /*
     * If you open /flash/image or /flash/tarball, or /flash/hash it will
     * interpret the open flags and perform whatever actions are required for
     * that update process.  The session returned can be used immediately for
     * sending data down, without requiring one to open the new active file.
     *
     * If you open the active flash image or active hash it will let you
     * overwrite pieces, depending on the state.
     * Once the verification process has started the active files cannot be
     * opened.
     */
    return false;
}

std::vector<uint8_t> FirmwareBlobHandler::read(uint16_t session,
                                               uint32_t offset,
                                               uint32_t requestedSize)
{
    /*
     * Currently, the design does not provide this with a function, however,
     * it will likely change to support reading data back.
     */
    return {};
}

bool FirmwareBlobHandler::write(uint16_t session, uint32_t offset,
                                const std::vector<uint8_t>& data)
{
    /*
     * This will do whatever behavior is expected by mechanism - likely will
     * just call the specific write handler.
     */
    return false;
}
bool FirmwareBlobHandler::writeMeta(uint16_t session, uint32_t offset,
                                    const std::vector<uint8_t>& data)
{
    /*
     * If the active session (image or hash) is over LPC, this allows
     * configuring it.  This option is only available before you start
     * writing data for the given item (image or hash).  This will return
     * false at any other part.
     */
    return false;
}
bool FirmwareBlobHandler::commit(uint16_t session,
                                 const std::vector<uint8_t>& data)
{
    /*
     * If this command is called on the session for the hash image, it'll
     * trigger a systemd service `verify_image.service` to attempt to verify
     * the image. Before doing this, if the transport mechanism is not IPMI
     * BT, it'll shut down the mechanism used for transport preventing the
     * host from updating anything.
     */
    return false;
}
bool FirmwareBlobHandler::close(uint16_t session)
{
    /*
     * Close must be called on the firmware image before triggering
     * verification via commit. Once the verification is complete, you can
     * then close the hash file.
     *
     * If the `verify_image.service` returned success, closing the hash file
     * will have a specific behavior depending on the update. If it's UBI,
     * it'll perform the install. If it's static layout, it'll do nothing. The
     * verify_image service in the static layout case is responsible for placing
     * the file in the correct staging position.
     */
    return false;
}
bool FirmwareBlobHandler::stat(uint16_t session, struct BlobMeta* meta)
{
    /*
     * Return the supported mechanisms if it's the handler blob_id, versus
     * the active one.
     */
    return false;
}
bool FirmwareBlobHandler::expire(uint16_t session)
{
    return false;
}
} // namespace blobs
