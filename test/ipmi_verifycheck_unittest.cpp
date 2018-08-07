#include "ipmi.hpp"

#include "updater_mock.hpp"

#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::StrictMock;

// ipmid.hpp isn't installed where we can grab it and this value is per BMC
// SoC.
#define MAX_IPMI_BUFFER 64

TEST(IpmiCheckVerifyTest, CallPassedOn)
{
    // This IPMI handler just bundles the response.

    StrictMock<UpdaterMock> updater;

    size_t dataLen;
    uint8_t request[MAX_IPMI_BUFFER] = {0};
    uint8_t reply[MAX_IPMI_BUFFER] = {0};

    dataLen = 1; // request is only the command.
    request[0] = FlashSubCmds::flashVerifyCheck;

    EXPECT_CALL(updater, checkVerify())
        .WillOnce(Return(VerifyCheckResponse::running));
    EXPECT_EQ(IPMI_CC_OK, checkVerify(&updater, request, reply, &dataLen));
    EXPECT_EQ(sizeof(uint8_t), dataLen);
    EXPECT_EQ(VerifyCheckResponse::running, reply[0]);
}
