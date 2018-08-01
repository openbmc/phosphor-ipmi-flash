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

TEST(IpmiStartTransferTest, InvalidRequestLengthReturnsFailure)
{
    // Verify that the request is sanity checked w.r.t length.

    StrictMock<UpdaterMock> updater;

    size_t dataLen;
    uint8_t request[MAX_IPMI_BUFFER] = {0};
    uint8_t reply[MAX_IPMI_BUFFER] = {0};

    struct StartTx tx;
    tx.cmd = FlashSubCmds::flashStartTransfer;
    tx.length = THIRTYTWO_MIB;
    std::memcpy(request, &tx, sizeof(tx));

    dataLen = sizeof(tx) - 1; // It's too small to be a valid packet.

    EXPECT_EQ(IPMI_CC_INVALID,
              startTransfer(&updater, request, reply, &dataLen));
}

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

    EXPECT_CALL(updater, start(NotNull())).WillOnce(Return(true));

    EXPECT_EQ(IPMI_CC_OK, startTransfer(&updater, request, reply, &dataLen));
}
