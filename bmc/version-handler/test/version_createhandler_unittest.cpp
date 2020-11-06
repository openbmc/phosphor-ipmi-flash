#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"
#include "version_handler.hpp"

#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{

TEST(VersionHandlerCanHandleTest, VerifyGoodInfoMapPasses)
{
    VersionInfoMap test;
    std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
    for (const auto& blobName : blobNames)
    {
        test.try_emplace(blobName,
                         VersionInfoPack(blobName,
                                         std::make_unique<VersionActionPack>(
                                             CreateTriggerMock()),
                                         CreateImageMock()));
    }
    auto handler = VersionBlobHandler::create(std::move(test));
    EXPECT_NE(handler, nullptr);
}

TEST(VersionCreateHandlerTest, VerifyEmptyInfoMapFails)
{
    VersionInfoMap test;
    auto handler = VersionBlobHandler::create(std::move(test));
    EXPECT_EQ(handler, nullptr);
}

TEST(VersionHandlerCanHandleTest, VerifyInfoMapWithNullActionPackFails)
{
    VersionInfoMap test;
    test.try_emplace("blob0",
                     VersionInfoPack("blob0", nullptr, CreateImageMock()));
    auto handler = VersionBlobHandler::create(std::move(test));
    EXPECT_EQ(handler, nullptr);
}

TEST(VersionHandlerCanHandleTest, VerifyInfoMapWithNullTriggerableActionFails)
{
    VersionInfoMap test;
    test.try_emplace(
        "blob0",
        VersionInfoPack("blob0", std::make_unique<VersionActionPack>(nullptr),
                        CreateImageMock()));
    auto handler = VersionBlobHandler::create(std::move(test));
    EXPECT_EQ(handler, nullptr);
}

TEST(VersionHandlerCanHandleTest, VerifyInfoMapWithNullImageHandlerFails)
{
    VersionInfoMap test;
    test.try_emplace(
        "blob0",
        VersionInfoPack(
            "blob0", std::make_unique<VersionActionPack>(CreateTriggerMock()),
            nullptr));
    auto handler = VersionBlobHandler::create(std::move(test));
    EXPECT_EQ(handler, nullptr);
}
} // namespace ipmi_flash
