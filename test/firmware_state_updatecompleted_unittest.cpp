/* The goal of these tests is to verify the behavior of all blob commands given
 * the current state is UpdateCompleted.  This state is achieved as an exit from
 * updateStarted.
 *
 * This can be reached with success or failure from an update, and is reached
 * via a stat() call from updatedStarted.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"
#include "status.hpp"
#include "util.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

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
 */

class FirmwareHandlerUpdateCompletedTest : public IpmiOnlyFirmwareStaticTest
{
};

/*
 * open(blob)
 */
TEST_F(FirmwareHandlerUpdateCompletedTest,
       AttemptToOpenFilesReturnsFailureAfterSuccess)
{
    /* In state updateCompleted a file is open, which means no others can be. */
    getToUpdateCompleted(ActionStatus::success);

    auto blobsToOpen = handler->getBlobIds();
    for (const auto& blob : blobsToOpen)
    {
        EXPECT_FALSE(handler->open(session + 1, flags, blob));
    }
}

/*
 * stat(session)
 */
TEST_F(FirmwareHandlerUpdateCompletedTest,
       CallingStatSessionAfterCompletedSuccessReturnsStateWithoutRechecking)
{
    getToUpdateCompleted(ActionStatus::success);
    EXPECT_CALL(*updateMockPtr, status()).Times(0);

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::committed;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(ActionStatus::success));

    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
    expectedState(FirmwareBlobHandler::UpdateState::updateCompleted);
}

TEST_F(FirmwareHandlerUpdateCompletedTest,
       CallingStatSessionAfterCompletedFailureReturnsStateWithoutRechecking)
{
    getToUpdateCompleted(ActionStatus::failed);
    EXPECT_CALL(*updateMockPtr, status()).Times(0);

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags | blobs::StateFlags::commit_error;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(ActionStatus::failed));

    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
    expectedState(FirmwareBlobHandler::UpdateState::updateCompleted);
}

/*
 * stat(blob)
 */
TEST_F(FirmwareHandlerUpdateCompletedTest, StatOnActiveImageReturnsFailure)
{
    getToUpdateCompleted(ActionStatus::success);

    ASSERT_TRUE(handler->canHandleBlob(activeImageBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeImageBlobId, &meta));
}

TEST_F(FirmwareHandlerUpdateCompletedTest, StatOnUpdateBlobReturnsFailure)
{
    getToUpdateCompleted(ActionStatus::success);

    ASSERT_TRUE(handler->canHandleBlob(updateBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(updateBlobId, &meta));
}

TEST_F(FirmwareHandlerUpdateCompletedTest, StatOnNormalBlobsReturnsSuccess)
{
    getToUpdateCompleted(ActionStatus::success);

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
 * There are the following calls (parameters may vary):
 * canHandleBlob(blob)
 * getBlobIds
 * deleteBlob(blob)
 * close(session)
 * writemeta(session)
 * write(session)
 * read(session)
 * commit(session)
 */

} // namespace
} // namespace ipmi_flash
