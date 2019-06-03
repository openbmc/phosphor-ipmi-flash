/* The goal of these tests is to verify the behavior of all blob commands given
 * the current state is updatePending.  This state is achieved as an exit from
 * verificationCompleted.
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
 * Testing canHandleBlob is uninteresting in this state.  Getting the BlobIDs
 * will inform what canHandleBlob will return.
 */

class FirmwareHandlerUpdatePendingTest : public IpmiOnlyFirmwareStaticTest
{
};

/*
 * There are the following calls (parameters may vary):
 * canHandleBlob(blob)
 * getBlobIds
 */
TEST_F(FirmwareHandlerUpdatePendingTest, GetBlobsListHasExpectedValues)
{
    getToUpdatePending();

    std::vector<std::string> expected = {updateBlobId, activeImageBlobId,
                                         hashBlobId, staticLayoutBlobId};
    EXPECT_THAT(handler->getBlobIds(), UnorderedElementsAreArray(expected));
}

/*
 * open(blob) - because updatePending is in a fileOpen==false state, one can
 * then open blobs. However, because we're in a special state, we will restrict
 * them s.t. they can only open the updateBlobId.
 */
TEST_F(FirmwareHandlerUpdatePendingTest,
       OpenUpdateBlobIdIsSuccessfulAndDoesNotChangeState)
{
    getToUpdatePending();

    /* Opening the update blob isn't interesting, except it's required for
     * commit() which triggers the update process.
     */
    EXPECT_TRUE(handler->open(session, flags, updateBlobId));
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);
}

TEST_F(FirmwareHandlerUpdatePendingTest, OpenAnyBlobOtherThanUpdateFails)
{
    getToUpdatePending();

    auto blobs = handler->getBlobIds();
    for (const auto& blob : blobs)
    {
        if (blob == updateBlobId)
        {
            continue;
        }
        EXPECT_FALSE(handler->open(session, flags, blob));
    }
}

/*
 * close(session) - close from this state is uninteresting.
 */
TEST_F(FirmwareHandlerUpdatePendingTest, CloseUpdateBlobDoesNotChangeState)
{
    /* Verify nothing changes when one just opens, then closes the updateBlobId.
     */
    getToUpdatePending();

    EXPECT_TRUE(handler->open(session, flags, updateBlobId));

    handler->close(session);

    expectedState(FirmwareBlobHandler::UpdateState::updatePending);
    EXPECT_TRUE(handler->canHandleBlob(updateBlobId));
}

/*
 * writemeta(session) - this will return failure.
 */
TEST_F(FirmwareHandlerUpdatePendingTest, WriteMetaToUpdateBlobReturnsFailure)
{
    getToUpdatePending();

    EXPECT_TRUE(handler->open(session, flags, updateBlobId));
    EXPECT_FALSE(handler->writeMeta(session, 0, {0x01}));
}

/*
 * write(session)
 */
TEST_F(FirmwareHandlerUpdatePendingTest, WriteToUpdateBlobReturnsFailure)
{
    getToUpdatePending();

    EXPECT_TRUE(handler->open(session, flags, updateBlobId));
    EXPECT_FALSE(handler->write(session, 0, {0x01}));
}

/*
 * read(session)
 */
TEST_F(FirmwareHandlerUpdatePendingTest, ReadFromUpdateBlobIdReturnsEmpty)
{
    getToUpdatePending();
    EXPECT_THAT(handler->read(session, 0, 1), IsEmpty());
}

/*
 * stat(blob)
 */
TEST_F(FirmwareHandlerUpdatePendingTest, StatOnActiveImageReturnsFailure)
{
    getToUpdatePending();
    ASSERT_TRUE(handler->canHandleBlob(activeImageBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeImageBlobId, &meta));
}

TEST_F(FirmwareHandlerUpdatePendingTest, StatOnUpdateBlobReturnsFailure)
{
    getToUpdatePending();
    ASSERT_TRUE(handler->canHandleBlob(updateBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(updateBlobId, &meta));
}

TEST_F(FirmwareHandlerUpdatePendingTest, StatOnNormalBlobsReturnsSuccess)
{
    getToUpdatePending();

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
 * stat(session)
 * In this case, you can open updateBlobId without changing state, therefore,
 * let's call stat() against a session against this file. This done, ahead of
 * commit() should report the state as "other."
 */
TEST_F(FirmwareHandlerUpdatePendingTest,
       SessionStatOnUpdateBlobIdReturnsFailure)
{
    getToUpdatePending();
    EXPECT_TRUE(handler->open(session, flags, updateBlobId));
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);

    blobs::BlobMeta meta, expectedMeta = {};
    expectedMeta.size = 0;
    expectedMeta.blobState = flags;
    expectedMeta.metadata.push_back(
        static_cast<std::uint8_t>(ActionStatus::unknown));

    EXPECT_TRUE(handler->stat(session, &meta));
    EXPECT_EQ(expectedMeta, meta);
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);
}

/*
 * commit(session)
 */
TEST_F(FirmwareHandlerUpdatePendingTest,
       CommitOnUpdateBlobTriggersUpdateAndChangesState)
{
    /* Commit triggers the update mechanism (similarly for the verifyBlobId) and
     * changes state to updateStarted.
     */
    getToUpdatePending();
    EXPECT_TRUE(handler->open(session, flags, updateBlobId));
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);

    EXPECT_CALL(*updateMockPtr, triggerUpdate()).WillOnce(Return(true));

    EXPECT_TRUE(handler->commit(session, {}));
    expectedState(FirmwareBlobHandler::UpdateState::updateStarted);
}

TEST_F(FirmwareHandlerUpdatePendingTest,
       CommitOnUpdateBlobTriggersUpdateAndReturnsFailureDoesNotChangeState)
{
    getToUpdatePending();
    EXPECT_TRUE(handler->open(session, flags, updateBlobId));
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);

    EXPECT_CALL(*updateMockPtr, triggerUpdate()).WillOnce(Return(false));

    EXPECT_FALSE(handler->commit(session, {}));
    expectedState(FirmwareBlobHandler::UpdateState::updatePending);
}

/*
 * TODO: deleteBlob(blob)
 */

} // namespace
} // namespace ipmi_flash
