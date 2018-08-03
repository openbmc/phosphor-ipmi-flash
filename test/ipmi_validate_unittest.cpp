#include "ipmi.hpp"

#include <gtest/gtest.h>

TEST(IpmiValidateTest, VerifyCommandMinimumLengths)
{
    struct TestCases
    {
        FlashSubCmds cmd,
        size_t len,
        bool expect
    };

    std::vector<TestCases> tests =
    {
        {FlashSubCmds::flashStartTransfer, sizeof(struct StartTx), true},
        {FlashSubCmds::flashStartTransfer, sizeof(struct StartTx) - 1, false},
        {FlashSubCmds::flashDataBlock, sizeof(struct ChunkHdr) - 1, false},
        {FlashSubCmds::flashDataBlock, sizeof(struct ChunkHdr), false},
        {FlashSubCmds::flashStartHash, sizeof(struct StartTx) - 1, false},
        {FlashSubCmds::flashHashData, sizeof(struct ChunkHdr) - 1, false},
    };

    for (const auto& test : tests)
    {
        bool result = validateRequestLength(test.cmd, test.len);
        EXPECT_EQ(result, test.expect);
    }
}
