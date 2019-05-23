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

} // namespace
} // namespace ipmi_flash
