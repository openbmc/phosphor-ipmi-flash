#include "flash-ipmi.hpp"

#include <cstdio>
#include <gtest/gtest.h>
#include <string>

#define THIRTYTWO_MIB 33554432

TEST(FlashIpmiHashStartTest, OutofSequenceFails)
{
    // Verify that the image must be started first. (can change).

    std::string name = std::tmpnam(nullptr);
    std::string name2 = std::tmpnam(nullptr);

    FlashUpdate updater(name, name2);
    EXPECT_FALSE(updater.startHash(THIRTYTWO_MIB));

    (void)std::remove(name.c_str());
    (void)std::remove(name2.c_str());
}

TEST(FlashIpmiHashStartTest, VerifyHashFileCreated)
{
    // Verify that it's happy.

    std::string name = std::tmpnam(nullptr);
    std::string name2 = std::tmpnam(nullptr);

    FlashUpdate updater(name, name2);
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));

    (void)std::remove(name.c_str());
    (void)std::remove(name2.c_str());
}
