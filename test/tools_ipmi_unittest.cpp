#include "internal_sys_mock.hpp"
#include "ipmi_errors.hpp"
#include "ipmi_handler.hpp"

namespace host_tool
{

using ::testing::_;
using ::testing::Return;

TEST(IpmiHandlerTest, OpenAllFails)
{
    /* Open against all device files fail. */
    internal::InternalSysMock sysMock;
    IpmiHandler ipmi(&sysMock);

    EXPECT_CALL(sysMock, open(_, _)).WillRepeatedly(Return(-1));
    EXPECT_THROW(ipmi.open(), IpmiException);
}

} // namespace host_tool
