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
    getToUpdateCompleted(UpdateStatus::success);

    auto blobsToOpen = handler->getBlobIds();
    for (const auto& blob : blobsToOpen)
    {
        EXPECT_FALSE(handler->open(session + 1, flags, blob));
    }
}

/*
 * There are the following calls (parameters may vary):
 * canHandleBlob(blob)
 * getBlobIds
 * deleteBlob(blob)
 * stat(blob)
 * stat(session)
 * close(session)
 * writemeta(session)
 * write(session)
 * read(session)
 * commit(session)
 */

} // namespace
} // namespace ipmi_flash
