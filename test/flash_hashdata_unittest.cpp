#include "flash-ipmi.hpp"

#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#define THIRTYTWO_MIB 33554432

class FlashIpmiHashDataTest : public ::testing::Test
{
  protected:
    FlashIpmiHashDataTest() = default;

    void SetUp() override
    {
        name = std::tmpnam(nullptr);
        name2 = std::tmpnam(nullptr);
    }
    void TearDown() override
    {
        (void)std::remove(name.c_str());
        (void)std::remove(name2.c_str());
    }

    std::string name;
    std::string name2;
};

TEST_F(FlashIpmiHashDataTest, CalledOutOfSequenceFails)
{
    // Verify that if you try to write to the hash before starting, it'll fail.
    // Presently, start() will open the hash file.

    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name, "", name2);
    EXPECT_FALSE(updater.hashData(0, bytes));
}

TEST_F(FlashIpmiHashDataTest, CalledWithDataSucceeds)
{
    // Verify the normal use case works.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name, "", name2);
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.hashData(0, bytes));

    auto file = std::fopen(name2.c_str(), "r");
    EXPECT_TRUE(file);

    uint8_t buffer[2];
    auto read = std::fread(buffer, 1, bytes.size(), file);
    EXPECT_EQ(read, bytes.size());
    EXPECT_EQ(0, std::memcmp(buffer, bytes.data(), bytes.size()));
    std::fclose(file);
}

TEST_F(FlashIpmiHashDataTest, CalledNonZeroOffsetSucceeds)
{
    // Skipping bytes in POSIX can create a sparse file.  In this case,
    // let's allow the behavior.  If we'd rather, we can have writeBlock
    // check that the offset is where we expect.

    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name, "", name2);
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.hashData(2, bytes));

    auto file = std::fopen(name2.c_str(), "r");
    EXPECT_TRUE(file);

    uint8_t buffer[4];
    auto read = std::fread(buffer, 1, sizeof(buffer), file);
    EXPECT_EQ(read, sizeof(buffer));
    EXPECT_EQ(0, std::memcmp(&buffer[2], bytes.data(), bytes.size()));
    std::fclose(file);
}
