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
 * Like the state verificationStarted, there is a file open in
 * verificationCompleted.  This state is transitioned to after a stat() command
 * indicates a successful verification.
 */

class FirmwareHandlerVerificationCompletedTest
    : public IpmiOnlyFirmwareStaticTest
{
  protected:
    void getToVerificationCompleted()
    {
        auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());
        EXPECT_CALL(imageMock, open(staticLayoutBlobId)).WillOnce(Return(true));
        EXPECT_TRUE(handler->open(session, flags, staticLayoutBlobId));
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

        EXPECT_CALL(*verifyMockPtr, checkVerificationState())
            .WillOnce(Return(VerifyCheckResponses::success));
        blobs::BlobMeta meta;
        EXPECT_TRUE(handler->stat(session, &meta));
        EXPECT_EQ(FirmwareBlobHandler::UpdateState::verificationCompleted,
                  realHandler->getCurrentState());
    }

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
};

/*
 * canHandleBlob(blob)
 * getBlobIds
 * deleteBlob(blob)
 *
 */

/*
 * stat(blob)
 */

/*
 * stat(session) - the verify blobid is open in this state, so stat on that once
 * completed should have no effect.
 *
 * open(blob) - all open should fail
 *
 * close(session) - close on the verify blobid:
 *   1. if successful adds update blob id, changes state to UpdatePending
 *   2. if unsuccessful doesn't add update blob id, changes state to?
 */

/*
 * writemeta(session) - write meta should fail.
 * write(session) - write should fail.
 * read(session) - read returns empty.
 * commit(session) - ?
 */

} // namespace
} // namespace ipmi_flash
