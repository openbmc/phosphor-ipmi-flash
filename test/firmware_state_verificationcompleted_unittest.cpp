/**
 * The goal of these tests is to verify the behavior of all blob commands given
 * the current state is verificationCompleted.  This state is achieved as a out
 * of verificationStarted.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"
#include "status.hpp"
#include "util.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::IsEmpty;
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
 * commit(session)
 *
 * Like the state verificationStarted, there is a file open in
 * verificationCompleted.  This state is transitioned to after a stat() command
 * indicates a successful verification.
 */

class FirmwareHandlerVerificationCompletedTest
    : public IpmiOnlyFirmwareStaticTest
{
  protected:
    void getToVerificationCompleted(VerifyCheckResponses checkResponse)
    {
        /* The hash was not sent up, as it's technically optional.  Therefore,
         * there is no active hash file.
         */
        EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));
        EXPECT_TRUE(handler->open(session, flags, staticLayoutBlobId));
        expectedState(FirmwareBlobHandler::UpdateState::uploadInProgress);

        EXPECT_CALL(imageMock, close()).WillRepeatedly(Return());
        handler->close(session);
        expectedState(FirmwareBlobHandler::UpdateState::verificationPending);

        EXPECT_TRUE(handler->open(session, flags, verifyBlobId));
        EXPECT_CALL(*verifyMockPtr, triggerVerification())
            .WillOnce(Return(true));

        EXPECT_TRUE(handler->commit(session, {}));
        expectedState(FirmwareBlobHandler::UpdateState::verificationStarted);

        EXPECT_CALL(*verifyMockPtr, checkVerificationState())
            .WillOnce(Return(checkResponse));
        blobs::BlobMeta meta;
        EXPECT_TRUE(handler->stat(session, &meta));
        expectedState(FirmwareBlobHandler::UpdateState::verificationCompleted);
    }

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
};

/* TODO: deleteBlob(blob) */

/*
 * canHandleBlob
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       OnVerificationCompleteSuccessUpdateBlobIdNotPresent)
{
    /* the uploadBlobId is only added on close() of the verifyBlobId.  This is a
     * consistent behavior with verifyBlobId only added when closing the image
     * or hash.
     */
    getToVerificationCompleted(VerifyCheckResponses::success);
    EXPECT_FALSE(handler->canHandleBlob(updateBlobId));
}

TEST_F(FirmwareHandlerVerificationCompletedTest,
       OnVerificationCompleteFailureUpdateBlobIdNotPresent)
{
    getToVerificationCompleted(VerifyCheckResponses::failed);
    EXPECT_FALSE(handler->canHandleBlob(updateBlobId));
}

/*
 * getBlobIds
 */
TEST_F(FirmwareHandlerVerificationCompletedTest, GetBlobIdsReturnsExpectedList)
{
    getToVerificationCompleted(VerifyCheckResponses::success);
    std::vector<std::string> expected = {verifyBlobId, hashBlobId,
                                         activeImageBlobId, staticLayoutBlobId};
    EXPECT_THAT(handler->getBlobIds(), UnorderedElementsAreArray(expected));
}

/*
 * stat(blob)
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       StatOnActiveImageReturnsFailure)
{
    getToVerificationCompleted(VerifyCheckResponses::success);
    ASSERT_TRUE(handler->canHandleBlob(activeImageBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeImageBlobId, &meta));
}

TEST_F(FirmwareHandlerVerificationCompletedTest,
       VerifyActiveHashIdMissingInThisCase)
{
    /* The path taken to get to this state never opened the hash blob Id, which
     * is fine.  But let's verify it behaved as intended.
     */
    getToVerificationCompleted(VerifyCheckResponses::success);
    EXPECT_FALSE(handler->canHandleBlob(activeHashBlobId));
}

/* TODO: Add sufficient warning that you can get to verificationCompleted
 * without ever opening the image blob id (or the tarball one).
 *
 * Although in this case, it's expected that any verification triggered would
 * certainly fail.  So, although it's possible, it's uninteresting.
 */

TEST_F(FirmwareHandlerVerificationCompletedTest, StatOnVerifyBlobReturnsFailure)
{
    getToVerificationCompleted(VerifyCheckResponses::success);
    ASSERT_TRUE(handler->canHandleBlob(verifyBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(verifyBlobId, &meta));
}

TEST_F(FirmwareHandlerVerificationCompletedTest,
       StatOnNormalBlobsReturnsSuccess)
{
    getToVerificationCompleted(VerifyCheckResponses::success);

    blobs::BlobMeta expected;
    expected.blobState = FirmwareBlobHandler::UpdateFlags::ipmi;
    expected.size = 0;

    std::vector<std::string> testBlobs = {staticLayoutBlobId, hashBlobId};
    for (const auto& blob : testBlobs)
    {
        ASSERT_TRUE(handler->canHandleBlob(blob));

        blobs::BlobMeta meta = {};
        EXPECT_TRUE(handler->stat(blob, &meta));
        EXPECT_EQ(expected, meta);
    }
}

/*
 * stat(session) - the verify blobid is open in this state, so stat on that once
 * completed should have no effect.
 */

/*
 * open(blob) - all open should fail
 */

/*
 * writemeta(session) - write meta should fail.
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       WriteMetaToVerifyBlobReturnsFailure)
{
    getToVerificationCompleted(VerifyCheckResponses::success);

    std::vector<std::uint8_t> bytes = {0x01, 0x02};
    EXPECT_FALSE(handler->writeMeta(session, 0, bytes));
}

/*
 * write(session) - write should fail.
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       WriteToVerifyBlobReturnsFailure)
{
    getToVerificationCompleted(VerifyCheckResponses::success);

    std::vector<std::uint8_t> bytes = {0x01, 0x02};
    EXPECT_FALSE(handler->write(session, 0, bytes));
}

/*
 * read(session) - read returns empty.
 */
TEST_F(FirmwareHandlerVerificationCompletedTest, ReadOfVerifyBlobReturnsEmpty)
{
    getToVerificationCompleted(VerifyCheckResponses::success);
    EXPECT_THAT(handler->read(session, 0, 1), IsEmpty());
}

/*
 * commit(session) - ?
 */

/*
 * close(session) - close on the verify blobid:
 *   1. if successful adds update blob id, changes state to UpdatePending
 *   2. if unsuccessful doesn't add update blob id, changes state to?
 */

} // namespace
} // namespace ipmi_flash
