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

/* canHandleBlob(blob)
 * getBlobIds
 */

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
 * open(blob)
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
