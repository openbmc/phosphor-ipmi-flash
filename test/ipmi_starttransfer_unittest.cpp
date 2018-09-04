#include "flash-ipmi.hpp"
#include "ipmi.hpp"
#include "updater_mock.hpp"

#include <cstring>

#include <gtest/gtest.h>

using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrictMock;

// ipmid.hpp isn't installed where we can grab it and this value is per BMC
// SoC.
#define MAX_IPMI_BUFFER 64
#define THIRTYTWO_MIB 33554432

TEST(IpmiStartTransferTest, ValidRequestBoringCase)
{
    // Verify that if the request is valid it calls into the flash updater.

    StrictMock<UpdaterMock> updater;

    size_t dataLen;
    uint8_t request[MAX_IPMI_BUFFER] = {0};
    uint8_t reply[MAX_IPMI_BUFFER] = {0};

    struct StartTx tx;
    tx.cmd = FlashSubCmds::flashStartTransfer;
    tx.length = THIRTYTWO_MIB;
    std::memcpy(request, &tx, sizeof(tx));

    dataLen = sizeof(tx);

    EXPECT_CALL(updater, start(THIRTYTWO_MIB)).WillOnce(Return(true));

    EXPECT_EQ(IPMI_CC_OK, startTransfer(&updater, request, reply, &dataLen));
    EXPECT_EQ(sizeof(uint8_t), dataLen);
    EXPECT_EQ(0, reply[0]);
}
