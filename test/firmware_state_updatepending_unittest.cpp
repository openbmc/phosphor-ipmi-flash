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
  protected:
    void getToUpdatePending()
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
            .WillOnce(Return(VerifyCheckResponses::success));
        blobs::BlobMeta meta;
        EXPECT_TRUE(handler->stat(session, &meta));
        expectedState(FirmwareBlobHandler::UpdateState::verificationCompleted);

        handler->close(session);
        expectedState(FirmwareBlobHandler::UpdateState::updatePending);
    }

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
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
 * TODO: deleteBlob(blob)
 */

/*
 * stat(blob)
 */

/*
 * stat(session)
 */

/*
 * write(session)
 */

/*
 * read(session)
 */

/*
 * commit(session)
 */

} // namespace
} // namespace ipmi_flash
