/**
 * The goal of these tests is to verify the behavior of all blob commands given
 * the current state is notYetStarted.  The initial state.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"

#include <gtest/gtest.h>

namespace ipmi_flash namespace
{

class FirmwareHandlerNotYetStartedTest : public FakeLpcFirmwareTest
{
};

/*
 * There are the following calls (parameters may vary):
 * Note: you cannot have a session yet, so only commands that don't take a
 * session parameter are valid. Once you open() from this state, we will vary
 * you transition out of this state (ensuring the above is true). Technically
 * the firmware handler receives the session number with open(), but the blob
 * manager is providing this normally.
 *
 * canHandleBlob
 * getBlobIds
 * deleteBlob
 * stat
 * open
 *
 * canHandleBlob is just a count check (or something similar) against what is
 * returned by getBlobIds.  It is tested in firmware_canhandle_unittest
 */

TEST_F(FirmwareHandlerNotYetStartedTest, GetBlobListValidateListContents)
{
    /* TODO: Presently /flash/verify is present from the beginning, however,
     * this is going to change to simplify handling of open().
     */
    std::vector<std::string> expectedBlobs = {};

    EXPECT_THAT(handler->getBlobIds(), ContainerEq(expectedBlobs));
}

} // namespace ipmi_flashnamespace
} // namespace ipmi_flash
