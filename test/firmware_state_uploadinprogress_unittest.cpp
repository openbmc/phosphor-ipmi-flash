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

    std::vector<std::string> expectedAfterImage = {
        staticLayoutBlobId, hashBlobId, verifyBlobId, activeImageBlobId};
    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray(expectedAfterImage));

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

    std::vector<std::string> expectedAfterImage = {
        staticLayoutBlobId, hashBlobId, verifyBlobId, activeHashBlobId};
    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray(expectedAfterImage));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeHashBlobId, &meta));
}

/* TODO: stat(verifyblobid) only should exist once both /image and /hash have
 * closed, so add this test when this is the case. NOTE: /image or /tarball
 * should have the same effect.
 */

/*
 * stat(session)
 * open(blob)
 * close(session)
 * writemeta(session)
 * write(session)
 * read(session)
 */

} // namespace
} // namespace ipmi_flash
