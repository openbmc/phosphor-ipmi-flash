#include "flash-ipmi.hpp"

#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#define THIRTYTWO_MIB 33554432

class FlashIpmiFlashDataTest : public ::testing::Test
{
  protected:
    FlashIpmiFlashDataTest() = default;

    void SetUp() override
    {
        name = std::tmpnam(nullptr);
    }
    void TearDown() override
    {
        (void)std::remove(name.c_str());
    }

    std::string name;
};

TEST_F(FlashIpmiFlashDataTest, CalledOutOfSequenceFails)
{
    // Verify that there is sanity checking.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name);
    EXPECT_FALSE(updater.flashData(0, bytes));

    // Verify the file doesn't exist.
    auto file = std::fopen(name.c_str(), "r");
    EXPECT_FALSE(file);
}

TEST_F(FlashIpmiFlashDataTest, CalledWithDataSucceeds)
{
    // Verify that under normal usage it writes the bytes.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name);
    updater.start(THIRTYTWO_MIB);
    EXPECT_TRUE(updater.flashData(0, bytes));

    auto file = std::fopen(name.c_str(), "r");
    EXPECT_TRUE(file);

    uint8_t buffer[2];
    auto read = std::fread(buffer, 1, bytes.size(), file);
    EXPECT_EQ(read, bytes.size());
    EXPECT_EQ(0, std::memcmp(buffer, bytes.data(), bytes.size()));
    std::fclose(file);
}

TEST_F(FlashIpmiFlashDataTest, CalledNonZeroOffsetSucceeds)
{
    // Skipping bytes in POSIX can create a sparse file.  In this case,
    // let's allow the behavior.  If we'd rather, we can have writeBlock
    // check that the offset is where we expect.

    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name);
    updater.start(THIRTYTWO_MIB);
    EXPECT_TRUE(updater.flashData(2, bytes));

    auto file = std::fopen(name.c_str(), "r");
    EXPECT_TRUE(file);

    uint8_t buffer[4];
    auto read = std::fread(buffer, 1, sizeof(buffer), file);
    EXPECT_EQ(read, sizeof(buffer));
    EXPECT_EQ(0, std::memcmp(&buffer[2], bytes.data(), bytes.size()));
    std::fclose(file);
}
