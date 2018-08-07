#include "flash-ipmi.hpp"

#include <cstdio>
#include <gtest/gtest.h>
#include <sdbusplus/test/sdbus_mock.hpp>
#include <string>

#define THIRTYTWO_MIB 33554432

TEST(FlashIpmiStartTest, VerifiesFieldsAndAction)
{
    // The interface does not currently support failure injection, so let's
    // simply verify it does what we think it should.

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    std::string name = std::tmpnam(nullptr);

    FlashUpdate updater(std::move(bus_mock), name, "");
    updater.start(THIRTYTWO_MIB);

    auto file = std::fopen(name.c_str(), "r");
    EXPECT_TRUE(file);
    std::fclose(file);
    (void)std::remove(name.c_str());
}
