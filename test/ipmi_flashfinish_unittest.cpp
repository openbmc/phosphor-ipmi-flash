#include "flash-ipmi.hpp"
#include "ipmi.hpp"
#include "updater_mock.hpp"

#include <cstring>

#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::StrictMock;

// ipmid.hpp isn't installed where we can grab it and this value is per BMC
// SoC.
#define MAX_IPMI_BUFFER 64

TEST(IpmiFlashFinish, CallPassedOn)
{
    // The data finish command does no validation as the input is empty, and
    // simply relies on the flash manager to do work.

    StrictMock<UpdaterMock> updater;

    size_t dataLen;
    uint8_t request[MAX_IPMI_BUFFER] = {0};
    uint8_t reply[MAX_IPMI_BUFFER] = {0};

    dataLen = 1; // request is only the command.
    request[0] = FlashSubCmds::flashDataFinish;

    EXPECT_CALL(updater, flashFinish()).WillOnce(Return(true));
    EXPECT_EQ(IPMI_CC_OK, dataFinish(&updater, request, reply, &dataLen));
    EXPECT_EQ(sizeof(uint8_t), dataLen);
    EXPECT_EQ(0, reply[0]);
}
