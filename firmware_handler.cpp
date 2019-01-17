/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "firmware_handler.hpp"

#include "image_handler.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <phosphor-logging/log.hpp>
#include <string>
#include <vector>

using namespace phosphor::logging;

namespace blobs
{
// systemd service to kick start a target.
static constexpr auto systemdService = "org.freedesktop.systemd1";
static constexpr auto systemdRoot = "/org/freedesktop/systemd1";
static constexpr auto systemdInterface = "org.freedesktop.systemd1.Manager";
static constexpr auto verifyTarget = "verify_image.service";

const std::string FirmwareBlobHandler::verifyBlobID = "/flash/verify";
const std::string FirmwareBlobHandler::hashBlobID = "/flash/hash";
const std::string FirmwareBlobHandler::activeImageBlobID =
    "/flash/active/image";
const std::string FirmwareBlobHandler::activeHashBlobID = "/flash/active/hash";

std::unique_ptr<GenericBlobInterface>
    FirmwareBlobHandler::CreateFirmwareBlobHandler(
        sdbusplus::bus::bus&& bus, const std::vector<HandlerPack>& firmwares,
        const std::vector<DataHandlerPack>& transports)
{
    /* There must be at least one. */
    if (!firmwares.size())
    {
        log<level::ERR>("Must provide at least one firmware handler.");
        return nullptr;
    }
    if (!transports.size())
    {
        return nullptr;
    }

    std::vector<std::string> blobs;
    for (const auto& item : firmwares)
    {
        blobs.push_back(item.blobName);
    }
    blobs.push_back(verifyBlobID); /* Add blob_id to always exist. */

    if (0 == std::count(blobs.begin(), blobs.end(), hashBlobID))
    {
        return nullptr;
    }

    std::uint16_t bitmask = 0;
    for (const auto& item : transports)
    {
        /* TODO: can use std::accumulate() unless I'm mistaken. :D */
        bitmask |= item.bitmask;
    }

    return std::make_unique<FirmwareBlobHandler>(std::move(bus), firmwares,
                                                 blobs, transports, bitmask);
}

/* Check if the path is in our supported list (or active list). */
bool FirmwareBlobHandler::canHandleBlob(const std::string& path)
{
    if (std::count(blobIDs.begin(), blobIDs.end(), path))
    {
        return true;
    }

    return false;
}

/*
 * Grab the list of supported firmware.
 *
 * If there's an open firmware session, it'll already be present in the
 * list as "/flash/active/image", and if the hash has started,
 * "/flash/active/hash" regardless of mechanism.  This is done in the open
 * comamnd, no extra work is required here.
 */
std::vector<std::string> FirmwareBlobHandler::getBlobIds()
{
    return blobIDs;
}

/*
 * Per the design, this mean abort, and this will trigger whatever
 * appropriate actions are required to abort the process.
 *
 * You cannot delete a blob that has an open handle in the system, therefore
 * this is never called if there's an open session.  Guaranteed by the blob
 * manager.
 */
bool FirmwareBlobHandler::deleteBlob(const std::string& path)
{
    const std::string* toDelete;

    /* You cannot delete the verify blob -- trying to delete it, currently has
     * no impact.
     * TODO: Should trying to delete this cause an abort?
     */
    if (path == verifyBlobID)
    {
        return false;
    }

    if (path == hashBlobID || path == activeHashBlobID)
    {
        /* They're deleting the hash. */
        toDelete = &activeHashBlobID;
    }
    else
    {
        /* They're deleting the image. */
        toDelete = &activeImageBlobID;
    }

    auto it = std::find_if(
        blobIDs.begin(), blobIDs.end(),
        [toDelete](const auto& iter) { return (iter == *toDelete); });
    if (it == blobIDs.end())
    {
        /* Somehow they've asked to delete something we didn't say we could
         * handle.
         */
        return false;
    }

    blobIDs.erase(it);

    /* TODO: Handle aborting the process and fixing up the state. */

    return true;
}

/*
 * Stat on the files will return information such as what supported
 * transport mechanisms are available.
 *
 * Stat on an active file or hash will return information such as the size
 * of the data cached, and any additional pertinent information.  The
 * blob_state on the active files will return the state of the update.
 */
bool FirmwareBlobHandler::stat(const std::string& path, struct BlobMeta* meta)
{
    /* We know we support this path because canHandle is called ahead */
    if (path == verifyBlobID)
    {
        /* We need to return information for the verify state -- did they call
         * commit() did things start?
         */
    }
    else if (path == activeImageBlobID)
    {
        /* We need to return information for the image that's staged. */
    }
    else if (path == activeHashBlobID)
    {
        /* We need to return information for the hash that's staged. */
    }
    else
    {
        /* They are requesting information about the generic blob_id. */
        meta->blobState = bitmask;
        meta->size = 0;

        /* The generic blob_ids state is only the bits related to the transport
         * mechanisms.
         */
        return true;
    }

    return false;
}

/*
 * Return stat information on an open session.  It therefore must be an active
 * handle to either the active image or active hash.
 *
 * The stat() and sessionstat() commands will return the same information in
 * many cases, therefore the logic will be combined.
 *
 * TODO: combine the logic for stat and sessionstat().
 */
bool FirmwareBlobHandler::stat(uint16_t session, struct BlobMeta* meta)
{
    auto item = lookup.find(session);
    if (item == lookup.end())
    {
        return false;
    }

    /* The blobState here relates to an active sesion, so we should return the
     * flags used to open this session.
     */
    meta->blobState = item->second->flags;
    /* The size here refers to the size of the file -- of something analagous.
     */
    meta->size = item->second->imageHandler->getSize();

    /* The metadata blob returned comes from the data handler... it's used for
     * instance, in P2A bridging to get required information about the mapping,
     * and is the "opposite" of the lpc writemeta requirement.
     */
    meta->metadata.clear();
    if (item->second->dataHandler)
    {
        auto bytes = item->second->dataHandler->readMeta();
        meta->metadata.insert(meta->metadata.begin(), bytes.begin(),
                              bytes.end());
    }

    /* TODO: During things like verification, etc, we can report the state as
     * committed, etc, so we'll need to do that.
     */

    return true;
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
    /* Check that they've opened for writing - read back not currently
     * supported.
     */
    if ((flags & OpenFlags::write) == 0)
    {
        return false;
    }

    /* Is the verification process underway? */
    if (state == UpdateState::verificationStarted)
    {
        return false;
    }

    /* Is there an open session already? We only allow one at a time.
     *
     * TODO: Temporarily using a simple boolean flag until there's a full
     * session object to check.
     *
     * Further on this, if there's an active session to the hash we don't allow
     * re-opening the image, and if we have the image open, we don't allow
     * opening the hash.  This design decision may be re-evaluated, and changed
     * to only allow one session per object type (of the two types).  But,
     * consider if the hash is open, do we want to allow writing to the image?
     * And why would we?  But, really, the point of no-return is once the
     * verification process has begun -- which is done via commit() on the hash
     * blob_id, we no longer want to allow updating the contents.
     */
    if (fileOpen)
    {
        return false;
    }

    /* Handle opening the verifyBlobId --> we know the image and hash aren't
     * open because of the fileOpen check.
     *
     * The file must be opened for writing, but no transport mechanism specified
     * since it's irrelevant.
     */
    if (path == verifyBlobID)
    {
        /* In this case, there's no image handler to use, or data handler,
         * simply set up a session.
         */
        verifyImage.flags = flags;
        verifyImage.state = Session::State::open;

        lookup[session] = &verifyImage;

        fileOpen = true;

        return true;
    }

    /* There are two abstractions at play, how you get the data and how you
     * handle that data. such that, whether the data comes from the PCI bridge
     * or LPC bridge is not connected to whether the data goes into a static
     * layout flash update or a UBI tarball.
     */

    /* Check the flags for the transport mechanism: if none match we don't
     * support what they request.
     */
    if ((flags & bitmask) == 0)
    {
        return false;
    }

    /* 2) there isn't, so what are they opening? */
    if (path == activeImageBlobID)
    {
        /* 2a) are they opening the active image? this can only happen if they
         * already started one (due to canHandleBlob's behavior).
         */
        return false;
    }
    else if (path == activeHashBlobID)
    {
        /* 2b) are they opening the active hash? this can only happen if they
         * already started one (due to canHandleBlob's behavior).
         */
        return false;
    }

    /* How are they expecting to copy this data? */
    auto d = std::find_if(
        transports.begin(), transports.end(),
        [&flags](const auto& iter) { return (iter.bitmask & flags); });
    if (d == transports.end())
    {
        return false;
    }

    /* We found the transport handler they requested, no surprise since
     * above we verify they selected at least one we wanted.
     */

    /* Elsewhere I do this check by checking "if ::ipmi" because that's the
     * only non-external data pathway -- but this is just a more generic
     * approach to that.
     */
    if (d->handler)
    {
        /* If the data handler open call fails, open fails. */
        if (!d->handler->open())
        {
            return false;
        }
    }

    /* Do we have a file handler for the type of file they're opening.
     * Note: This should only fail if something is somehow crazy wrong.
     * Since the canHandle() said yes, and that's tied into the list of explicit
     * firmware handers (and file handlers, like this'll know where to write the
     * tarball, etc).
     */
    auto h = std::find_if(
        handlers.begin(), handlers.end(),
        [&path](const auto& iter) { return (iter.blobName == path); });
    if (h == handlers.end())
    {
        return false;
    }

    /* Ok, so we found a handler that matched, so call open() */
    if (!h->handler->open(path))
    {
        return false;
    }

    Session* curr;
    const std::string* active;

    if (path == hashBlobID)
    {
        /* 2c) are they opening the /flash/hash ? (to start the process) */
        curr = &activeHash;
        active = &activeHashBlobID;
    }
    else
    {
        curr = &activeImage;
        active = &activeImageBlobID;
    }

    curr->flags = flags;
    curr->dataHandler = d->handler;
    curr->imageHandler = h->handler;
    curr->state = Session::State::open;

    lookup[session] = curr;

    blobIDs.push_back(*active);

    fileOpen = true;

    return true;
}

/**
 * The write command really just grabs the data from wherever it is and sends it
 * to the image handler.  It's the image handler's responsibility to deal with
 * the data provided.
 *
 * This receives a session from the blob manager, therefore it is always called
 * between open() and close().
 */
bool FirmwareBlobHandler::write(uint16_t session, uint32_t offset,
                                const std::vector<uint8_t>& data)
{
    auto item = lookup.find(session);
    if (item == lookup.end())
    {
        return false;
    }

    /* Prevent writing during verification. */
    if (state == UpdateState::verificationStarted)
    {
        return false;
    }

    std::vector<std::uint8_t> bytes;

    if (item->second->flags & UpdateFlags::ipmi)
    {
        bytes = data;
    }
    else
    {
        /* little endian required per design, and so on, but TODO: do endianness
         * with boost.
         */
        struct ExtChunkHdr header;

        if (data.size() != sizeof(header))
        {
            return false;
        }

        std::memcpy(&header, data.data(), data.size());
        bytes = item->second->dataHandler->copyFrom(header.length);
    }

    return item->second->imageHandler->write(offset, bytes);
}

/*
 * If the active session (image or hash) is over LPC, this allows
 * configuring it.  This option is only available before you start
 * writing data for the given item (image or hash).  This will return
 * false at any other part. -- the lpc handler portion will know to return
 * false.
 */
bool FirmwareBlobHandler::writeMeta(uint16_t session, uint32_t offset,
                                    const std::vector<uint8_t>& data)
{
    auto item = lookup.find(session);
    if (item == lookup.end())
    {
        return false;
    }

    if (item->second->flags & UpdateFlags::ipmi)
    {
        return false;
    }

    return item->second->dataHandler->writeMeta(data);
}

/*
 * If this command is called on the session for the verifyBlobID, it'll
 * trigger a systemd service `verify_image.service` to attempt to verify
 * the image.
 *
 * For this file to have opened, the other two must be closed, which means any
 * out-of-band transport mechanism involved is closed.
 */
bool FirmwareBlobHandler::commit(uint16_t session,
                                 const std::vector<uint8_t>& data)
{
    auto item = lookup.find(session);
    if (item == lookup.end())
    {
        return false;
    }

    /* You can only commit on the verifyBlodId */
    if (item->second->activePath != verifyBlobID)
    {
        return false;
    }

    /* Can only be called once per verification. */
    if (state == UpdateState::verificationStarted)
    {
        return false;
    }

    /* Set state to committing. */
    item->second->flags |= StateFlags::committing;

    return triggerVerification();
}

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
bool FirmwareBlobHandler::close(uint16_t session)
{
    auto item = lookup.find(session);
    if (item == lookup.end())
    {
        return false;
    }

    /* Are you closing the verify blob? */
    if (item->second->activePath == verifyBlobID)
    {
        /* If they close this blob before verification finishes, that's an
         * abort.
         * TODO: implement this, for now just let them close the file.
         */
        if (state == UpdateState::verificationStarted)
        {
            return false;
        }
    }

    if (item->second->dataHandler)
    {
        item->second->dataHandler->close();
    }
    if (item->second->imageHandler)
    {
        item->second->imageHandler->close();
    }

    item->second->state = Session::State::closed;
    /* Do not delete the active blob_id from the list of blob_ids, because that
     * blob_id indicates there is data stored.  Delete will destroy it.
     */

    lookup.erase(item);

    fileOpen = false;

    /* TODO: implement other aspects of closing out a session, such as changing
     * global firmware state.
     */
    return true;
}

bool FirmwareBlobHandler::expire(uint16_t session)
{
    return false;
}

/*
 * Currently, the design does not provide this with a function, however,
 * it will likely change to support reading data back.
 */
std::vector<uint8_t> FirmwareBlobHandler::read(uint16_t session,
                                               uint32_t offset,
                                               uint32_t requestedSize)
{
    return {};
}

bool FirmwareBlobHandler::triggerVerification()
{
    auto method = bus.new_method_call(systemdService, systemdRoot,
                                      systemdInterface, "StartUnit");
    method.append(verifyTarget);
    method.append("replace");

    try
    {
        bus.call_noreply(method);
        state = UpdateState::verificationStarted;
    }
    catch (const sdbusplus::exception::SdBusError& ex)
    {
        /* TODO: Once logging supports unit-tests, add a log message to test
         * this failure.
         */
        return false;
    }

    return true;
}

} // namespace blobs
