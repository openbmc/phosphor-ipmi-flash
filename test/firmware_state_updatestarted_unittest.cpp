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
