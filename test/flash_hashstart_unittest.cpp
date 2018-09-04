#include "flash-ipmi.hpp"

#include <cstdio>
#include <sdbusplus/test/sdbus_mock.hpp>
#include <string>

#include <gtest/gtest.h>

#define THIRTYTWO_MIB 33554432

TEST(FlashIpmiHashStartTest, OutofSequenceFails)
{
    // Verify that the image must be started first. (can change).

    std::string name = std::tmpnam(nullptr);
    std::string name2 = std::tmpnam(nullptr);

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), name, "", name2);
    EXPECT_FALSE(updater.startHash(THIRTYTWO_MIB));

    (void)std::remove(name.c_str());
    (void)std::remove(name2.c_str());
}

TEST(FlashIpmiHashStartTest, VerifyHashFileCreated)
{
    // Verify that it's happy.

    std::string name = std::tmpnam(nullptr);
    std::string name2 = std::tmpnam(nullptr);

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), name, "", name2);
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));

    (void)std::remove(name.c_str());
    (void)std::remove(name2.c_str());
}
