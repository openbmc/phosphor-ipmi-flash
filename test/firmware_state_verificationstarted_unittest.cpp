/* The goal of these tests is to verify the behavior of all blob commands given
 * the current state is verificationStarted.  This state is achieved as a out of
 * verificationPending.
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
 * Testing canHandleBlob is uninteresting in this state.  Getting the BlobIDs
 * will inform what canHandleBlob will return.
 */

class FirmwareHandlerVerificationStartedTest : public IpmiOnlyFirmwareStaticTest
{
  protected:
    void getToVerificationStarted(const std::string& blobId)
    {
        auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());
        EXPECT_CALL(imageMock, open(blobId)).WillOnce(Return(true));
        EXPECT_TRUE(handler->open(session, flags, blobId));
        EXPECT_EQ(FirmwareBlobHandler::UpdateState::uploadInProgress,
                  realHandler->getCurrentState());
        EXPECT_CALL(imageMock, close()).WillRepeatedly(Return());
        handler->close(session);
        EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationPending,
                  realHandler->getCurrentState());

        EXPECT_TRUE(handler->open(session, flags, verifyBlobId));
        EXPECT_CALL(*verifyMockPtr, triggerVerification())
            .WillOnce(Return(true));

        EXPECT_TRUE(handler->commit(session, {}));
        EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationStarted,
                  realHandler->getCurrentState());
    }

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
};

/*
 * stat(session)
 */
TEST_F(FirmwareHandlerVerificationStartedTest,
       StatOnVerifyBlobIdAfterCommitChecksStateAndReturnsRunning)
{
    getToVerificationStarted(staticLayoutBlobId);
    EXPECT_CALL(*verifyMockPtr, checkVerificationState())
        .WillOnce(Return(VerifyCheckResponses::running));

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::committing;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(VerifyCheckResponses::running));

    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
}

TEST_F(FirmwareHandlerVerificationStartedTest,
       StatOnVerifyBlobIdAfterCommitCheckStateAndReturnsFailed)
{
    /* If the returned state from the verification handler is failed, it sets
     * commit_error and transitions to verificationCompleted.
     */
    getToVerificationStarted(staticLayoutBlobId);
    EXPECT_CALL(*verifyMockPtr, checkVerificationState())
        .WillOnce(Return(VerifyCheckResponses::failed));

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::commit_error;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(VerifyCheckResponses::failed));

    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());
    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);

    EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationCompleted,
              realHandler->getCurrentState());
}

TEST_F(FirmwareHandlerVerificationStartedTest,
       StatOnVerifyBlobIdAfterCommitCheckStateAndReturnsSuccess)
{
    /* If the returned state from the verification handler is success, it sets
     * committed and transitions to verificationCompleted.
     */
    getToVerificationStarted(staticLayoutBlobId);
    EXPECT_CALL(*verifyMockPtr, checkVerificationState())
        .WillOnce(Return(VerifyCheckResponses::success));

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::committed;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(VerifyCheckResponses::success));

    auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());
    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);

    EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationCompleted,
              realHandler->getCurrentState());
}

/* TODO: Once verificationCompleted is the state, canHandleBlob should accept
 * updateBlobId.
 */

/* TODO:
 * canHandleBlob(blob)
 *
 * getBlobIds
 *
 * deleteBlob(blob)
 */

/*
 * stat(blob)
 */
TEST_F(FirmwareHandlerVerificationStartedTest, StatOnActiveImageReturnsFailure)
{
    getToVerificationStarted(staticLayoutBlobId);
    ASSERT_TRUE(handler->canHandleBlob(activeImageBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeImageBlobId, &meta));
}

TEST_F(FirmwareHandlerVerificationStartedTest, StatOnActiveHashReturnsFailure)
{
    getToVerificationStarted(hashBlobId);
    ASSERT_TRUE(handler->canHandleBlob(activeHashBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeHashBlobId, &meta));
}

TEST_F(FirmwareHandlerVerificationStartedTest, StatOnVerifyBlobReturnsFailure)
{
    /* the verifyBlobId is available starting at verificationPending. */
    getToVerificationStarted(staticLayoutBlobId);
    ASSERT_TRUE(handler->canHandleBlob(verifyBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(verifyBlobId, &meta));
}

TEST_F(FirmwareHandlerVerificationStartedTest, StatOnNormalBlobsReturnsSuccess)
{
    getToVerificationStarted(staticLayoutBlobId);

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
 * writemeta(session)
 */
TEST_F(FirmwareHandlerVerificationStartedTest,
       WriteMetaOnVerifySessionReturnsFailure)
{
    getToVerificationStarted(staticLayoutBlobId);

    std::vector<std::uint8_t> bytes = {0x01, 0x02};
    EXPECT_FALSE(handler->writeMeta(session, 0, bytes));
}

/*
 * write(session)
 */
TEST_F(FirmwareHandlerVerificationStartedTest,
       WriteOnVerifySessionReturnsFailure)
{
    getToVerificationStarted(staticLayoutBlobId);

    std::vector<std::uint8_t> bytes = {0x01, 0x02};
    EXPECT_FALSE(handler->write(session, 0, bytes));
}

/*
 * open(blob) - there is nothing you can open, this state has an open file.
 */
TEST_F(FirmwareHandlerVerificationStartedTest,
       AttemptToOpenImageFileReturnsFailure)
{
    /* Attempt to open a file one normally can open, however, as there is
     * already a file open, this will fail.
     */
    getToVerificationStarted(staticLayoutBlobId);

    EXPECT_FALSE(handler->open(session + 1, flags, staticLayoutBlobId));
}

/*
 * read(session)
 */
TEST_F(FirmwareHandlerVerificationStartedTest, ReadOfVerifyBlobReturnsEmpty)
{
    getToVerificationStarted(staticLayoutBlobId);
    EXPECT_THAT(handler->read(session, 0, 32), IsEmpty());
}

/*
 * commit(session)
 */
TEST_F(FirmwareHandlerVerificationStartedTest,
       CommitOnVerifyDuringVerificationHasNoImpact)
{
    getToVerificationStarted(staticLayoutBlobId);
    EXPECT_TRUE(handler->commit(session, {}));
    EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationStarted,
              realHandler->getCurrentState());
}

/*
 * close(session) - close while state if verificationStarted without calling
 * stat first will abort.
 */

} // namespace
} // namespace ipmi_flash
