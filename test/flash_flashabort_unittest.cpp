#include "flash-ipmi.hpp"

#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <sdbusplus/test/sdbus_mock.hpp>
#include <string>

#define THIRTYTWO_MIB 33554432

using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrEq;

TEST(FlashIpmiAbortTest, VerifyItDeletesAndStopsServiced)
{
    // Verify that once everything is started, the image and hash are deleted
    // and it will try to stop the verification service.

    std::string iname = std::tmpnam(nullptr);
    std::string vname = std::tmpnam(nullptr);
    std::string hname = std::tmpnam(nullptr);

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), iname, vname, hname);

    std::vector<uint8_t> ibytes = {0xaa, 0x55};
    std::vector<uint8_t> hbytes = {0xcc, 0x65};

    // Send the bytes up for the image.
    EXPECT_TRUE(updater.start(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.flashData(0, ibytes));
    EXPECT_TRUE(updater.flashFinish());

    // Send the bytes up for the hash.
    EXPECT_TRUE(updater.startHash(THIRTYTWO_MIB));
    EXPECT_TRUE(updater.hashData(0, hbytes));
    EXPECT_TRUE(updater.hashFinish());

    // Verify the image bytes.
    auto file = std::fopen(iname.c_str(), "r");
    EXPECT_TRUE(file);
    uint8_t buffer[2];
    auto read = std::fread(buffer, 1, ibytes.size(), file);
    EXPECT_EQ(read, ibytes.size());
    EXPECT_EQ(0, std::memcmp(buffer, ibytes.data(), ibytes.size()));
    std::fclose(file);

    // Verify the hash bytes.
    file = std::fopen(hname.c_str(), "r");
    EXPECT_TRUE(file);
    read = std::fread(buffer, 1, hbytes.size(), file);
    EXPECT_EQ(read, hbytes.size());
    EXPECT_EQ(0, std::memcmp(buffer, hbytes.data(), hbytes.size()));
    std::fclose(file);

    EXPECT_CALL(sdbus_mock,
                sd_bus_message_new_method_call(
                    IsNull(), NotNull(), StrEq("org.freedesktop.systemd1"),
                    StrEq("/org/freedesktop/systemd1"),
                    StrEq("org.freedesktop.systemd1.Manager"),
                    StrEq("StopUnit")))
        .WillOnce(Return(0));

    // Send the abort.
    EXPECT_TRUE(updater.abortUpdate());

    // Verify the files are gone.
    file = std::fopen(iname.c_str(), "r");
    EXPECT_FALSE(file);
    file = std::fopen(hname.c_str(), "r");
    EXPECT_FALSE(file);
}
