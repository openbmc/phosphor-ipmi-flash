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
#define HASH_SIZE 512

TEST(IpmiStartHashTest, ValidBoringCaseReturnsSuccess)
{
    // This is also mostly a pass-thru command such that the real validation is
    // done elsewhere.

    StrictMock<UpdaterMock> updater;

    size_t dataLen;
    uint8_t request[MAX_IPMI_BUFFER] = {0};
    uint8_t reply[MAX_IPMI_BUFFER] = {0};

    struct StartTx tx;
    tx.cmd = FlashSubCmds::flashStartHash;
    tx.length = HASH_SIZE;
    std::memcpy(request, &tx, sizeof(tx));

    dataLen = sizeof(tx);

    EXPECT_CALL(updater, startHash(HASH_SIZE)).WillOnce(Return(true));

    EXPECT_EQ(IPMI_CC_OK, startHash(&updater, request, reply, &dataLen));
    EXPECT_EQ(sizeof(uint8_t), dataLen);
    EXPECT_EQ(0, reply[0]);
}
