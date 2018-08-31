#include "ipmi.hpp"

#include <gtest/gtest.h>

namespace
{

void EqualFunctions(IpmiFlashHandler lhs, IpmiFlashHandler rhs)
{
    EXPECT_FALSE(lhs == nullptr);
    EXPECT_FALSE(rhs == nullptr);
    ipmi_ret_t (*const* lPtr)(UpdateInterface*, const uint8_t*, uint8_t*,
                              size_t*) =
        lhs.target<ipmi_ret_t (*)(UpdateInterface*, const uint8_t*, uint8_t*,
                                  size_t*)>();
    ipmi_ret_t (*const* rPtr)(UpdateInterface*, const uint8_t*, uint8_t*,
                              size_t*) =
        rhs.target<ipmi_ret_t (*)(UpdateInterface*, const uint8_t*, uint8_t*,
                                  size_t*)>();
    EXPECT_TRUE(lPtr);
    EXPECT_TRUE(rPtr);
    EXPECT_EQ(*lPtr, *rPtr);
    return;
}
} // namespace

TEST(IpmiCommandTest, VerifyCommandReturnsExpected)
{
    // Given a subcommand that's valid, make sure it returns the expected
    // pointer.

    auto result = getCommandHandler(FlashSubCmds::flashHashFinish);
    EqualFunctions(hashFinish, result);
}

TEST(IpmiCommandTest, VerifyInvalidCommandReturnsNull)
{
    // Given a subcommand that's invalid, make sure it returns the nullptr.

    auto result = getCommandHandler(static_cast<FlashSubCmds>(25));
    EXPECT_EQ(result, nullptr);
}
