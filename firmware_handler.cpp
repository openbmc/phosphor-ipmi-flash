#include "firmware_handler.hpp"

#include "image_handler.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

const std::string FirmwareBlobHandler::hashBlobID = "/flash/hash";
const std::string FirmwareBlobHandler::activeImageBlobID =
    "/flash/active/image";
const std::string FirmwareBlobHandler::activeHashBlobID = "/flash/active/hash";

std::unique_ptr<GenericBlobInterface>
    FirmwareBlobHandler::CreateFirmwareBlobHandler(
        const std::vector<HandlerPack>& firmwares, std::uint16_t transports)
{
    /* There must be at least one. */
    if (!firmwares.size())
    {
        return nullptr;
    }

    std::vector<std::string> blobs;
    for (const auto& item : firmwares)
    {
        blobs.push_back(item.blobName);
    }
    blobs.push_back(hashBlobID);

    return std::make_unique<FirmwareBlobHandler>(firmwares, blobs, transports);
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

    /* We know we support this path because canHandle is called ahead */
    if (path == FirmwareBlobHandler::activeImageBlobID)
    {
        /* We need to return information for the image that's staged. */
    }
    else if (path == FirmwareBlobHandler::activeHashBlobID)
    {
        /* We need to return information for the hash that's staged. */
    }
    else
    {
        /* They are requesting information about the generic blob_id. */
        meta->blobState = transports;
        meta->size = 0;

        /* The generic blob_ids state is only the bits related to the transport
         * mechanisms. */
        return true;
    }

    return false;
}

/*
 * If you open /flash/image or /flash/tarball, or /flash/hash it will
 * interpret the open flags and perform whatever actions are required for
 * that update process.  The session returned can be used immediately for
 * sending data down, without requiring one to open the new active file.
 *
 * If you open the active flash image or active hash it will let you
 * overwrite pieces, depending on the state.
 *
 * Once the verification process has started the active files cannot be
 * opened.
 *
 * You can only have one open session at a time.  Which means, you can only
 * have one file open at a time.  Trying to open the hash blob_id while you
 * still have the flash image blob_id open will fail.  Opening the flash
 * blob_id when it is already open will fail.
 */
bool FirmwareBlobHandler::open(uint16_t session, uint16_t flags,
                               const std::string& path)
{
    /* Check that they've opened for writing - read back not supported. */
    if ((flags & OpenFlags::write) == 0)
    {
        return false;
    }

    /* TODO: Is the verification process underway? */

    /* Is there an open session already? We only allow one at a time.
     * TODO: Temporarily using a simple boolean flag until there's a full
     * session object to check.
     */
    if (fileOpen)
    {
        return false;
    }

    /* There are two abstractions at play, how you get the data and how you
     * handle that data. such that, whether the data comes from the PCI bridge
     * or LPC bridge is not connected to whether the data goes into a static
     * layout flash update or a UBI tarball.
     */

    /* Check the flags for the transport mechanism: if none match we don't
     * support what they request. */
    if ((flags & transports) == 0)
    {
        return false;
    }

    /* 2) there isn't, so what are they opening? */
    if (path == activeImageBlobID)
    {
        /* 2a) are they opening the active image? this can only happen if they
         * already started one (due to canHandleBlob's behavior). */
    }
    else if (path == activeHashBlobID)
    {
        /* 2b) are they opening the active hash? this can only happen if they
         * already started one (due to canHandleBlob's behavior). */
    }
    else if (path == hashBlobID)
    {
        /* 2c) are they opening the /flash/hash ? (to start the process) */
    }
    else
    {
        /* 2d) are they opening the /flash/tarball ? (to start the UBI process)
         */
        /* 2e) are they opening the /flash/image ? (to start the process) */
        /* 2...) are they opening the /flash/... ? (to start the process) */

        auto h = std::find_if(
            handlers.begin(), handlers.end(),
            [&path](const auto& iter) { return (iter.blobName == path); });
        if (h != handlers.end())
        {
            /* Ok, so we found a handler that matched, so call open() */
            if (h->handler->open(path))
            {
                /* open() succeeded. */
            }
        }

        /* TODO: Actually handle storing this information. */
    }

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
     * Return session specific information.
     */
    return false;
}
bool FirmwareBlobHandler::expire(uint16_t session)
{
    return false;
}
} // namespace blobs
