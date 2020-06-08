#include "internal_sys_mock.hpp"
#include "pciaccess_mock.hpp"

#include <gtest/gtest.h>

namespace host_tool
{
namespace
{

TEST(PciHandleTest, empty)
{
    PciAccessMock pciMock;

    (void)pciMock;
}

} // namespace
} // namespace host_tool
