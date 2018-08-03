#include "flash-ipmi.hpp"
#include "ipmi.hpp"

#include "updater_mock.hpp"

#include <cstring>
#include <gtest/gtest.h>

using ::testing::ElementsAreArray;
using ::testing::Return;
using ::testing::StrictMock;

// ipmid.hpp isn't installed where we can grab it and this value is per BMC
// SoC.
#define MAX_IPMI_BUFFER 64

TEST(IpmiHashData, DataReceivedIsPassedOnOk)
{
    // If valid data was passed in, it'll pass it onto the update handler.

    StrictMock<UpdaterMock> updater;

    size_t dataLen;
    uint8_t request[MAX_IPMI_BUFFER] = {0};
    uint8_t reply[MAX_IPMI_BUFFER] = {0};

    struct ChunkHdr tx;
    tx.cmd = FlashSubCmds::flashHashData;
    tx.offset = 0x00;
    std::memcpy(request, &tx, sizeof(tx));

    uint8_t bytes[] = {0x04, 0x05};
    std::memcpy(&request[sizeof(struct ChunkHdr)], bytes, sizeof(bytes));

    dataLen = sizeof(struct ChunkHdr) + sizeof(bytes);

    EXPECT_CALL(updater, hashData(0x00, ElementsAreArray(bytes, sizeof(bytes))))
        .WillOnce(Return(true));

    EXPECT_EQ(IPMI_CC_OK, hashBlock(&updater, request, reply, &dataLen));
    EXPECT_EQ(sizeof(uint8_t), dataLen);
    EXPECT_EQ(0, reply[0]);
}
