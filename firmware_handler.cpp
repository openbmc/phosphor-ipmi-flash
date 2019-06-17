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
#include "status.hpp"
#include "util.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <phosphor-logging/log.hpp>
#include <string>
#include <vector>

using namespace phosphor::logging;

namespace ipmi_flash
{

std::unique_ptr<blobs::GenericBlobInterface>
    FirmwareBlobHandler::CreateFirmwareBlobHandler(
        const std::vector<HandlerPack>& firmwares,
        const std::vector<DataHandlerPack>& transports,
        std::unique_ptr<TriggerableActionInterface> verification,
        std::unique_ptr<TriggerableActionInterface> update)
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

    if (0 == std::count(blobs.begin(), blobs.end(), hashBlobId))
    {
        return nullptr;
    }

    std::uint16_t bitmask = 0;
    for (const auto& item : transports)
    {
        /* TODO: can use std::accumulate() unless I'm mistaken. :D */
        bitmask |= item.bitmask;
    }

    return std::make_unique<FirmwareBlobHandler>(
        firmwares, blobs, transports, bitmask, std::move(verification),
        std::move(update));
}

/* Check if the path is in our supported list (or active list). */
bool FirmwareBlobHandler::canHandleBlob(const std::string& path)
{
    return (std::count(blobIDs.begin(), blobIDs.end(), path) > 0);
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
 */
bool FirmwareBlobHandler::deleteBlob(const std::string& path)
{
    /* This cannot be called if you have an open session to the path.
     * You can have an open session to verify/update/hash/image, but not active*
     *
     * Therefore, if this is called, it's either on a blob that isn't presently
     * open.  However, there could be open blobs, so we need to close all open
     * sessions. This closing on our is an invalid handler behavior.  Therefore,
     * we cannot close an active session.  To enforce this, we only allow
     * deleting if there isn't a file open.
     */
    if (fileOpen)
    {
        return false;
    }

    /* only includes states where fileOpen == false */
    switch (state)
    {
        case UpdateState::notYetStarted:
            /* Trying to delete anything at this point has no effect and returns
             * false.
             */
            return false;
        case UpdateState::verificationPending:
            abortProcess();
            return true;
        case UpdateState::updatePending:
            abortProcess();
            return true;
        default:
            break;
    }

    return false;
}

/*
 * Stat on the files will return information such as what supported
 * transport mechanisms are available.
 *
 * Stat on an active file or hash will return information such as the size
 * of the data cached, and any additional pertinent information.  The
 * blob_state on the active files will return the state of the update.
 */
bool FirmwareBlobHandler::stat(const std::string& path, blobs::BlobMeta* meta)
{
    /* We know we support this path because canHandle is called ahead */
    if (path == verifyBlobId || path == activeImageBlobId ||
        path == activeHashBlobId || path == updateBlobId)
    {
        /* These blobs are placeholders that indicate things, or allow actions,
         * but are not stat-able as-is.
         */
        return false;
    }

    /* They are requesting information about the generic blob_id. */
    meta->blobState = bitmask;
    meta->size = 0;

    /* The generic blob_ids state is only the bits related to the transport
     * mechanisms.
     */
    return true;
}

ActionStatus FirmwareBlobHandler::getActionStatus()
{
    ActionStatus value = ActionStatus::unknown;

    switch (state)
    {
        case UpdateState::verificationPending:
            value = ActionStatus::unknown;
            break;
        case UpdateState::verificationStarted:
            value = verification->status();
            lastVerificationStatus = value;
            break;
        case UpdateState::verificationCompleted:
            value = lastVerificationStatus;
            break;
        case UpdateState::updatePending:
            value = ActionStatus::unknown;
            break;
        case UpdateState::updateStarted:
            value = update->status();
            lastUpdateStatus = value;
            break;
        case UpdateState::updateCompleted:
            value = lastUpdateStatus;
            break;
        default:
            break;
    }

    return value;
}

/*
 * Return stat information on an open session.  It therefore must be an active
 * handle to either the active image or active hash.
 */
bool FirmwareBlobHandler::stat(uint16_t session, blobs::BlobMeta* meta)
{
    auto item = lookup.find(session);
    if (item == lookup.end())
    {
        return false;
    }

    /* The size here refers to the size of the file -- of something analagous.
     */
    meta->size = (item->second->imageHandler)
                     ? item->second->imageHandler->getSize()
                     : 0;

    meta->metadata.clear();

    if (item->second->activePath == verifyBlobId ||
        item->second->activePath == updateBlobId)
    {
        ActionStatus value = getActionStatus();

        meta->metadata.push_back(static_cast<std::uint8_t>(value));

        /* Change the firmware handler's state and the blob's stat value
         * depending.
         */
        if (value == ActionStatus::success || value == ActionStatus::failed)
        {
            if (item->second->activePath == verifyBlobId)
            {
                state = UpdateState::verificationCompleted;
            }
            else
            {
                /* item->second->activePath == updateBlobId */
                state = UpdateState::updateCompleted;
            }

            item->second->flags &= ~blobs::StateFlags::committing;

            if (value == ActionStatus::success)
            {
                item->second->flags |= blobs::StateFlags::committed;
            }
            else
            {
                item->second->flags |= blobs::StateFlags::commit_error;
            }
        }
    }

    /* The blobState here relates to an active sesion, so we should return the
     * flags used to open this session.
     */
    meta->blobState = item->second->flags;

    /* The metadata blob returned comes from the data handler... it's used for
     * instance, in P2A bridging to get required information about the mapping,
     * and is the "opposite" of the lpc writemeta requirement.
     */
    if (item->second->dataHandler)
    {
        auto bytes = item->second->dataHandler->readMeta();
        meta->metadata.insert(meta->metadata.begin(), bytes.begin(),
                              bytes.end());
    }

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
    /* Is there an open session already? We only allow one at a time.
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

    /* The active blobs are only meant to indicate status that something has
     * opened the image file or the hash file.
     */
    if (path == activeImageBlobId || path == activeHashBlobId)
    {
        /* 2a) are they opening the active image? this can only happen if they
         * already started one (due to canHandleBlob's behavior).
         */
        /* 2b) are they opening the active hash? this can only happen if they
         * already started one (due to canHandleBlob's behavior).
         */
        return false;
    }

    /* Check that they've opened for writing - read back not currently
     * supported.
     */
    if ((flags & blobs::OpenFlags::write) == 0)
    {
        return false;
    }

    /* Because canHandleBlob is called before open, we know that if they try to
     *  open the verifyBlobId, they're in a state where it's present.
     */

    switch (state)
    {
        case UpdateState::uploadInProgress:
            /* Unreachable code because if it's started a file is open. */
            break;
        case UpdateState::verificationPending:
            /* Handle opening the verifyBlobId --> we know the image and hash
             * aren't open because of the fileOpen check.  They can still open
             * other files from this state to transition back into
             * uploadInProgress.
             *
             * The file must be opened for writing, but no transport mechanism
             * specified since it's irrelevant.
             */
            if (path == verifyBlobId)
            {
                verifyImage.flags = flags;

                lookup[session] = &verifyImage;

                fileOpen = true;
                return true;
            }
            break;
        case UpdateState::verificationStarted:
        case UpdateState::verificationCompleted:
            /* Unreachable code because if it's started a file is open. */
            return false;
        case UpdateState::updatePending:
        {
            /* When in this state, they can only open the updateBlobId */
            if (path == updateBlobId)
            {
                updateImage.flags = flags;

                lookup[session] = &updateImage;

                fileOpen = true;
                return true;
            }
            else
            {
                return false;
            }
        }
        case UpdateState::updateStarted:
        case UpdateState::updateCompleted:
            /* Unreachable code because if it's started a file is open. */
            break;
        default:
            break;
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

    if (path == hashBlobId)
    {
        /* 2c) are they opening the /flash/hash ? (to start the process) */
        curr = &activeHash;
        active = &activeHashBlobId;
    }
    else
    {
        curr = &activeImage;
        active = &activeImageBlobId;
    }

    curr->flags = flags;
    curr->dataHandler = d->handler;
    curr->imageHandler = h->handler;

    lookup[session] = curr;

    addBlobId(*active);
    removeBlobId(verifyBlobId);

    state = UpdateState::uploadInProgress;
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

    /* Prevent writing to the verification or update blobs. */
    if (item->second->activePath == verifyBlobId ||
        item->second->activePath == updateBlobId)
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

    /* Prevent writing meta to the verification blob (it has no data handler).
     */
    if (item->second->dataHandler)
    {
        return item->second->dataHandler->writeMeta(data);
    }

    return false;
}

/*
 * If this command is called on the session for the verifyBlobId, it'll
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

    /* You can only commit on the verifyBlodId or updateBlobId */
    if (item->second->activePath != verifyBlobId &&
        item->second->activePath != updateBlobId)
    {
        std::fprintf(stderr, "path: '%s' not expected for commit\n",
                     item->second->activePath.c_str());
        return false;
    }

    switch (state)
    {
        case UpdateState::verificationPending:
            /* Set state to committing. */
            item->second->flags |= blobs::StateFlags::committing;
            return triggerVerification();
        case UpdateState::verificationStarted:
            /* Calling repeatedly has no effect within an update process. */
            return true;
        case UpdateState::verificationCompleted:
            /* Calling after the verification process has completed returns
             * failure. */
            return false;
        case UpdateState::updatePending:
            item->second->flags |= blobs::StateFlags::committing;
            return triggerUpdate();
        case UpdateState::updateStarted:
            /* Calling repeatedly has no effect within an update process. */
            return true;
        default:
            return false;
    }
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

    switch (state)
    {
        case UpdateState::uploadInProgress:
            /* They are closing a data pathway (image, tarball, hash). */
            state = UpdateState::verificationPending;

            /* Add verify blob ID now that we can expect it. */
            addBlobId(verifyBlobId);
            break;
        case UpdateState::verificationPending:
            /* They haven't triggered, therefore closing is uninteresting.
             */
            break;
        case UpdateState::verificationStarted:
            /* Abort without checking to see if it happened to finish. Require
             * the caller to stat() deliberately.
             */
            abortVerification();
            abortProcess();
            break;
        case UpdateState::verificationCompleted:
            if (lastVerificationStatus == ActionStatus::success)
            {
                state = UpdateState::updatePending;
                addBlobId(updateBlobId);
                removeBlobId(verifyBlobId);
            }
            else
            {
                /* TODO: Verification failed, what now? */
            }
            break;
        case UpdateState::updatePending:
            /* They haven't triggered the update, therefore this is
             * uninteresting. */
            break;
        case UpdateState::updateStarted:
            /* Abort without checking to see if it happened to finish. Require
             * the caller to stat() deliberately.
             */
            abortUpdate();
            abortProcess();
            break;
        case UpdateState::updateCompleted:
            if (lastUpdateStatus == ActionStatus::failed)
            {
                /* TODO: lOG something? */
            }

            state = UpdateState::notYetStarted;
            removeBlobId(updateBlobId);
            removeBlobId(activeImageBlobId);
            removeBlobId(activeHashBlobId);
            break;
        default:
            break;
    }

    if (item->second->dataHandler)
    {
        item->second->dataHandler->close();
    }
    if (item->second->imageHandler)
    {
        item->second->imageHandler->close();
    }

    lookup.erase(item);
    fileOpen = false;
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

void FirmwareBlobHandler::abortProcess()
{
    /* Closing of open files is handled from close() -- Reaching here from
     * delete may never be supported.
     */
    removeBlobId(verifyBlobId);
    removeBlobId(updateBlobId);
    removeBlobId(activeImageBlobId);
    removeBlobId(activeHashBlobId);

    state = UpdateState::notYetStarted;
}

void FirmwareBlobHandler::abortVerification()
{
    verification->abort();
}

bool FirmwareBlobHandler::triggerVerification()
{
    bool result = verification->trigger();
    if (result)
    {
        state = UpdateState::verificationStarted;
    }

    return result;
}

void FirmwareBlobHandler::abortUpdate()
{
    update->abort();
}

bool FirmwareBlobHandler::triggerUpdate()
{
    bool result = update->trigger();
    if (result)
    {
        state = UpdateState::updateStarted;
    }

    return result;
}

} // namespace ipmi_flash
