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
    // Verify that there is sanity checking
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name, name2);
    EXPECT_FALSE(updater.hashFinish());

    // Verify the file doesn't exist.
    auto file = std::fopen(name2.c_str(), "r");
    EXPECT_FALSE(file);
}

TEST_F(FlashIpmiHashDataTest, CalledInSequenceSucceeds)
{
    // Verify that under normal usage it closes the file.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name, name2);
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.hashFinish());

    // Verify we can open the file, so we know it didn't get deleted.
    auto file = std::fopen(name2.c_str(), "r");
    EXPECT_TRUE(file);
    std::fclose(file);
}

TEST_F(FlashIpmiHashDataTest, CalledTwiceFails)
{
    // Verify that under normal usage it closes the file, and therefore cannot
    // be closed twice.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    FlashUpdate updater(name, name2);
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.hashFinish());

    EXPECT_FALSE(updater.hashFinish());

    // Verify we can open the file, so we know it didn't get deleted.
    auto file = std::fopen(name2.c_str(), "r");
    EXPECT_TRUE(file);
    std::fclose(file);
}
