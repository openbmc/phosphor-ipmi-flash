/**
 * The goal of these tests is to verify the behavior of all blob commands given
 * the current state is uploadInProgress.  This state is achieved when an image
 * or hash blob is opened and the handler is expected to receive bytes.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::Return;
using ::testing::UnorderedElementsAreArray;

/*
 * There are the following calls (parameters may vary):
 * canHandleBlob(blob)
 * getBlobIds
 * deleteBlob(blob)
 * stat(blob)
 * stat(session)
 * open(blob)
 * close(session)
 * writemeta(session)
 * write(session)
 * read(session)
 *
 * Testing canHandleBlob is uninteresting in this state.  Getting the BlobIDs
 * will inform what canHandleBlob will return.
 */
class FirmwareHandlerUploadInProgressTest : public IpmiOnlyFirmwareStaticTest
{
};

TEST_F(FirmwareHandlerUploadInProgressTest, GetBlobIdsVerifyOutputActiveImage)
{
    /* Opening the image file will add the active image blob id */
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, staticLayoutBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    std::vector<std::string> expectedAfterImage = {
        staticLayoutBlobId, hashBlobId, verifyBlobId, activeImageBlobId};
    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray(expectedAfterImage));
}

TEST_F(FirmwareHandlerUploadInProgressTest, GetBlobIdsVerifyOutputActiveHash)
{
    /* Opening the image file will add the active image blob id */
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(hashBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, hashBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    std::vector<std::string> expectedAfterImage = {
        staticLayoutBlobId, hashBlobId, verifyBlobId, activeHashBlobId};
    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray(expectedAfterImage));
}

/* TODO: Try deleting some blobs -- in this state it will depend on what the
 * blob id is, but it's not yet implemented
 */

/*
 * stat(blob)
 */
TEST_F(FirmwareHandlerUploadInProgressTest, StatOnActiveImageReturnsFailure)
{
    /* you cannot call stat() on the active image or the active hash or the
     * verify blob.
     */
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, staticLayoutBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeImageBlobId, &meta));
}

TEST_F(FirmwareHandlerUploadInProgressTest, StatOnActiveHashReturnsFailure)
{
    /* this test is separate from the active image one so that the state doesn't
     * change from close.
     */
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(hashBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, hashBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeHashBlobId, &meta));
}

TEST_F(FirmwareHandlerUploadInProgressTest, StatOnNormalBlobsReturnsSuccess)
{
    /* Calling stat() on the normal blobs (not the active) ones will work and
     * return the same information as in the notYetStarted state.
     */
    blobs::BlobMeta expected;
    expected.blobState = FirmwareBlobHandler::UpdateFlags::ipmi;
    expected.size = 0;

    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, staticLayoutBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    std::vector<std::string> testBlobs = {staticLayoutBlobId, hashBlobId};
    for (const auto& blob : testBlobs)
    {
        blobs::BlobMeta meta = {};
        EXPECT_TRUE(handler->stat(blob, &meta));
        EXPECT_EQ(expected, meta);
    }
}

/* TODO: stat(verifyblobid) only should exist once both /image and /hash have
 * closed, so add this test when this is the case. NOTE: /image or /tarball
 * should have the same effect.
 */

/*
 * stat(session)
 */
TEST_F(FirmwareHandlerUploadInProgressTest,
       CallingStatOnActiveImageOrHashSessionReturnsDetails)
{
    /* This test will verify that the underlying image handler is called with
     * this stat, in addition to the normal information.
     */
    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(session, flags, staticLayoutBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    EXPECT_CALL(imageMock, getSize()).WillOnce(Return(32));

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 32;
    expectedMeta.blobState =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
}

/*
 * open(blob) - While any blob is open, all other fail.
 */
TEST_F(FirmwareHandlerUploadInProgressTest, OpeningHashFileWhileImageOpenFails)
{
    /* To be in this state, something must be open (and specifically either an
     * active image (or tarball) or the hash file. Also verifies you can't just
     * re-open the currently open file.
     */
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, staticLayoutBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    std::vector<std::string> blobsToTry = {
        hashBlobId, activeImageBlobId, activeHashBlobId, staticLayoutBlobId};
    for (const auto& blob : blobsToTry)
    {
        EXPECT_FALSE(handler->open(2, flags, blob));
    }
}

TEST_F(FirmwareHandlerUploadInProgressTest, OpeningImageFileWhileHashOpenFails)
{
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    EXPECT_CALL(imageMock, open(hashBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, hashBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    std::vector<std::string> blobsToTry = {
        hashBlobId, activeImageBlobId, activeHashBlobId, staticLayoutBlobId};
    for (const auto& blob : blobsToTry)
    {
        EXPECT_FALSE(handler->open(2, flags, blob));
    }
}

/*
 * close(session) - closing the hash or image will trigger a state transition to
 * verificationPending.
 * TODO: This state transition should add the verifyBlobId.  This will test that
 * it's there, but this test doesn't verify that it only just now appeared.
 *
 * NOTE: Re-opening /flash/image will transition back to uploadInProgress, but
 * that is verified in the verificationPending::open tests.
 */
TEST_F(FirmwareHandlerUploadInProgressTest,
       ClosingImageFileTransitionsToVerificationPending)
{
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    /* TODO: uncomment this when verify is properly added. */
    // EXPECT_FALSE(handler->canHandleBlob(verifyBlobId));

    EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, staticLayoutBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    handler->close(1);
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationPending,
              realHandler->getCurrentState());

    EXPECT_TRUE(handler->canHandleBlob(verifyBlobId));
}

TEST_F(FirmwareHandlerUploadInProgressTest,
       ClosingHashFileTransitionsToVerificationPending)
{
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());

    /* TODO: uncomment this when verify is properly added. */
    // EXPECT_FALSE(handler->canHandleBlob(verifyBlobId));

    EXPECT_CALL(imageMock, open(hashBlobId)).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(1, flags, hashBlobId));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
              realHandler->getCurrentState());

    handler->close(1);
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationPending,
              realHandler->getCurrentState());

    EXPECT_TRUE(handler->canHandleBlob(verifyBlobId));
}

/*
 * writemeta(session)
 * write(session)
 * read(session)
 */

} // namespace
} // namespace ipmi_flash
