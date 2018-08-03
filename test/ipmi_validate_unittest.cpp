#include "ipmi.hpp"

#include <gtest/gtest.h>

// ipmid.hpp isn't installed where we can grab it and this value is per BMC
// SoC.
#define MAX_IPMI_BUFFER 64

TEST(IpmiValidateTest, BoringValidateCheckReturnsTrue)
{
    // Verify the validate check can return true when it's happy.
    auto cmd = FlashSubCmds::flashStartTransfer;
    size_t dataLen = sizeof(struct StartTx);
    EXPECT_TRUE(validateRequestLength(cmd, dataLen));
}

TEST(IpmiValidateTest, InvalidLengthStartTransferReturnsFalse)
{
    // Verify that the request is sanity checked w.r.t length.
    auto cmd = FlashSubCmds::flashStartTransfer;
    size_t dataLen = sizeof(struct StartTx) - 1;
    EXPECT_FALSE(validateRequestLength(cmd, dataLen));
}

TEST(IpmiValidateTest, InvalidLengthDataBlockReturnsFalse)
{
    // This request isn't large enough to be well-formed.
    auto cmd = FlashSubCmds::flashDataBlock;
    size_t dataLen = sizeof(struct ChunkHdr) - 1;
    EXPECT_FALSE(validateRequestLength(cmd, dataLen));
}

TEST(IpmiValidateTest, DataBlockNoDataReturnsFalse)
{
    // If the request has no data, it's invalid, returns failure.
    auto cmd = FlashSubCmds::flashDataBlock;
    size_t dataLen = sizeof(struct ChunkHdr);
    EXPECT_FALSE(validateRequestLength(cmd, dataLen));
}

TEST(IpmiValidateTest, StartHashInvalidReturnsFalse)
{
    // Verify the request is sanity checked w.r.t length.
    auto cmd = FlashSubCmds::flashStartHash;
    size_t dataLen = sizeof(struct StartTx) - 1;
    EXPECT_FALSE(validateRequestLength(cmd, dataLen));
}

TEST(IpmiValidateTest, InvalidLengthHashBlockReturnsFalse)
{
    // This request isn't large enough to be well-formed.
    auto cmd = FlashSubCmds::flashHashData;
    size_t dataLen = sizeof(struct ChunkHdr) - 1;
    EXPECT_FALSE(validateRequestLength(cmd, dataLen));
}
