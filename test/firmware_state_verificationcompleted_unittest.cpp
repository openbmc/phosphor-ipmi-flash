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
TEST_F(FirmwareHandlerVerificationCompletedTest,
       SessionStatOnVerifyAfterSuccessDoesNothing)
{
    /* Every time you stat() once it's triggered, it checks the state again
     * until it's completed.
     */
    getToVerificationCompleted(VerifyCheckResponses::success);
    EXPECT_CALL(*verifyMockPtr, checkVerificationState()).Times(0);

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::committed;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(VerifyCheckResponses::success));

    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
    expectedState(FirmwareBlobHandler::UpdateState::verificationCompleted);
}

TEST_F(FirmwareHandlerVerificationCompletedTest,
       SessionStatOnVerifyAfterFailureDoesNothing)
{
    getToVerificationCompleted(VerifyCheckResponses::failed);
    EXPECT_CALL(*verifyMockPtr, checkVerificationState()).Times(0);

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::commit_error;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(VerifyCheckResponses::failed));

    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
    expectedState(FirmwareBlobHandler::UpdateState::verificationCompleted);
}

/*
 * open(blob) - all open should fail
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       OpeningAnyBlobAvailableFailsAfterSuccess)
{
    getToVerificationCompleted(VerifyCheckResponses::success);

    auto blobs = handler->getBlobIds();
    for (const auto& blob : blobs)
    {
        EXPECT_FALSE(handler->open(session + 1, flags, blob));
    }
}

TEST_F(FirmwareHandlerVerificationCompletedTest,
       OpeningAnyBlobAvailableFailsAfterFailure)
{
    getToVerificationCompleted(VerifyCheckResponses::failed);

    auto blobs = handler->getBlobIds();
    for (const auto& blob : blobs)
    {
        EXPECT_FALSE(handler->open(session + 1, flags, blob));
    }
}

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
 * commit(session) - returns failure
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       CommitOnVerifyBlobAfterSuccessReturnsFailure)
{
    /* If you've started this'll return success, but if it's finished, it won't
     * let you try-again.
     */
    getToVerificationCompleted(VerifyCheckResponses::success);
    EXPECT_CALL(*verifyMockPtr, triggerVerification()).Times(0);

    EXPECT_FALSE(handler->commit(session, {}));
}

TEST_F(FirmwareHandlerVerificationCompletedTest,
       CommitOnVerifyBlobAfterFailureReturnsFailure)
{
    getToVerificationCompleted(VerifyCheckResponses::failed);
    EXPECT_CALL(*verifyMockPtr, triggerVerification()).Times(0);

    EXPECT_FALSE(handler->commit(session, {}));
}

/*
 * close(session) - close on the verify blobid:
 *   1. if successful adds update blob id, changes state to UpdatePending
 */
TEST_F(FirmwareHandlerVerificationCompletedTest,
       CloseAfterSuccessChangesStateAddsUpdateBlob)
{
    getToVerificationCompleted(VerifyCheckResponses::success);
    ASSERT_FALSE(handler->canHandleBlob(updateBlobId));

    handler->close(session);
    EXPECT_TRUE(handler->canHandleBlob(updateBlobId));
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);
}

/*
 * close(session) - close on the verify blobid:
 * TODO:  2. if unsuccessful doesn't add update blob id, changes state to?
 */

} // namespace
} // namespace ipmi_flash
