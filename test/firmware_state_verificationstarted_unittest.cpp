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
 * getBlobIds
 * deleteBlob(blob)
 * stat(blob)
 * open(blob) - there is nothing you can open, this state has an open file.
 * close(session)
 * writemeta(session)
 * write(session)
 * read(session)
 * commit(session)
 */

} // namespace
} // namespace ipmi_flash
