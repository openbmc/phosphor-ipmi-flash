#include "flash-ipmi.hpp"

#include <cstdio>
#include <fstream>
#include <sdbusplus/test/sdbus_mock.hpp>
#include <string>
#include <vector>

#include <gtest/gtest.h>

class FlashIpmiCheckVerifyTest : public ::testing::Test
{
  protected:
    FlashIpmiCheckVerifyTest() = default;

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

TEST_F(FlashIpmiCheckVerifyTest, VerifyItReturnsFileContents)
{
    // If the file is present, the value can be a string and we convert it to
    // an enum.

    struct Expect
    {
        std::string value;
        VerifyCheckResponse response;
    };

    std::vector<Expect> tests = {
        {"running", VerifyCheckResponse::running},
        {"success", VerifyCheckResponse::success},
        {"failed", VerifyCheckResponse::failed},
        {"asdf", VerifyCheckResponse::other},
    };

    std::string vname = std::tmpnam(nullptr);

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    FlashUpdate updater(std::move(bus_mock), name, vname);

    for (const auto& test : tests)
    {
        std::ofstream out;
        out.open(vname);
        out << test.value << std::endl;
        out.close();

        EXPECT_EQ(test.response, updater.checkVerify());

        (void)std::remove(vname.c_str());
    }
}
