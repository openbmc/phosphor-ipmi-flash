#include "bmc_update_mock.hpp"
#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

class FirmwareHandlerDeleteTest : public FakeLpcFirmwareTest
{
};

TEST_F(FirmwareHandlerDeleteTest, DeleteActiveHashSucceeds)
{
    /* Delete active image succeeds. */
    EXPECT_CALL(imageMock, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        hashBlobId));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(4, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));

    /* Set up close() expectations. */
    EXPECT_CALL(imageMock, close());
    EXPECT_TRUE(handler->close(0));

    currentBlobs = handler->getBlobIds();
    EXPECT_EQ(4, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));

    /* Delete the blob. */
    EXPECT_TRUE(handler->deleteBlob(activeHashBlobId));

    currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(0, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));
}

} // namespace ipmi_flash
