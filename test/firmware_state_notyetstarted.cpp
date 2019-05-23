/**
 * The goal of these tests is to verify the behavior of all blob commands given
 * the current state is notYetStarted.  The initial state.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

class FirmwareHandlerNotYetStartedTest : public FakeLpcFirmwareTest
{
};

} // namespace
} // namespace ipmi_flash
