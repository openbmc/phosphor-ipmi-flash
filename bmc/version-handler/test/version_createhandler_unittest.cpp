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

} // namespace ipmi_flash
