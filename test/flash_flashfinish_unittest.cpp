#include "flash-ipmi.hpp"

#include <cstdio>
#include <cstring>
#include <sdbusplus/test/sdbus_mock.hpp>
#include <string>
#include <vector>

#include <gtest/gtest.h>

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
    // Verify that there is sanity checking
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), name, "");
    EXPECT_FALSE(updater.flashFinish());

    // Verify the file doesn't exist.
    auto file = std::fopen(name.c_str(), "r");
    EXPECT_FALSE(file);
}

TEST_F(FlashIpmiFlashDataTest, CalledInSequenceSucceeds)
{
    // Verify that under normal usage it closes the file.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), name, "");
    updater.start(THIRTYTWO_MIB);
    EXPECT_TRUE(updater.flashFinish());

    // Verify we can open the file, so we know it didn't get deleted.
    auto file = std::fopen(name.c_str(), "r");
    EXPECT_TRUE(file);
    std::fclose(file);
}

TEST_F(FlashIpmiFlashDataTest, CalledTwiceFails)
{
    // Verify that under normal usage it closes the file, and therefore cannot
    // be closed twice.
    std::vector<uint8_t> bytes = {0xaa, 0x55};

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), name, "");
    updater.start(THIRTYTWO_MIB);
    EXPECT_TRUE(updater.flashFinish());

    EXPECT_FALSE(updater.flashFinish());

    // Verify we can open the file, so we know it didn't get deleted.
    auto file = std::fopen(name.c_str(), "r");
    EXPECT_TRUE(file);
    std::fclose(file);
}
