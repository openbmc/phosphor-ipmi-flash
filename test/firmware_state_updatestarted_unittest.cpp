/* The goal of these tests is to verify the behavior of all blob commands given
 * the current state is updateStarted.  This state is achieved as an exit from
 * updatePending.
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
 */

class FirmwareHandlerUpdateStartedTest : public IpmiOnlyFirmwareStaticTest
{
};

/*
 * open(blob)
 */
TEST_F(FirmwareHandlerUpdateStartedTest, AttemptToOpenFilesReturnsFailure)
{
    /* In state updateStarted a file is open, which means no others can be. */
    getToUpdateStarted();

    auto blobsToOpen = handler->getBlobIds();
    for (const auto& blob : blobsToOpen)
    {
        EXPECT_FALSE(handler->open(session + 1, flags, blob));
    }
}

/* canHandleBlob(blob)
 * getBlobIds
 */
TEST_F(FirmwareHandlerUpdateStartedTest, VerifyListOfBlobs)
{
    getToUpdateStarted();

    std::vector<std::string> expected = {updateBlobId, hashBlobId,
                                         activeImageBlobId, staticLayoutBlobId};
    EXPECT_THAT(handler->getBlobIds(), UnorderedElementsAreArray(expected));
}

/*
 * TODO: deleteBlob(blob)
 */

/*
 * stat(blob)
 */
TEST_F(FirmwareHandlerUpdateStartedTest, StatOnActiveImageReturnsFailure)
{
    getToUpdateStarted();

    ASSERT_TRUE(handler->canHandleBlob(activeImageBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(activeImageBlobId, &meta));
}

TEST_F(FirmwareHandlerUpdateStartedTest, StatOnUpdateBlobReturnsFailure)
{
    getToUpdateStarted();

    ASSERT_TRUE(handler->canHandleBlob(updateBlobId));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat(updateBlobId, &meta));
}

TEST_F(FirmwareHandlerUpdateStartedTest, StatOnNormalBlobsReturnsSuccess)
{
    getToUpdateStarted();

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
 */

/*
 * close(session)
 */

/*
 * writemeta(session)
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
