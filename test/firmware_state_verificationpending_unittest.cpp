/**
 * The goal of these tests is to verify the behavior of all blob commands given
 * the current state is verificationPending.  This state is achieved as a
 * transition out of uploadInProgress.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"
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

class FirmwareHandlerVerificationPendingTest : public IpmiOnlyFirmwareStaticTest
{
  protected:
    void getToVerificationPending(const std::string& blobId)
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
    }

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
};

/*
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

TEST_F(FirmwareHandlerVerificationPendingTest, VerifyBlobIdAvailableInState)
{
    /* Only in the verificationPending state (and later), should the
     * verifyBlobId be present. */

    /* TODO: Add this test in when the change is made. */
    // EXPECT_FALSE(handler->canHandleBlob(verifyBlobId));
    getToVerificationPending(staticLayoutBlobId);
    EXPECT_TRUE(handler->canHandleBlob(verifyBlobId));
}

} // namespace
} // namespace ipmi_flash
