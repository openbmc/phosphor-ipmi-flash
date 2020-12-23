#include "version_handler.hpp"
#include "version_mock.hpp"

#include <array>

#include <gtest/gtest.h>

namespace ipmi_flash
{

TEST(VersionHandlerCanHandleTest, VerifyGoodInfoMap)
{
    constexpr std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
    VersionBlobHandler handler(createMockVersionConfigs(blobNames));
    for (const auto& blobName : blobNames)
    {
        EXPECT_TRUE(handler.canHandleBlob(blobName));
    }
}

TEST(VersionHandlerEnumerateTest, VerifyGoodInfoMap)
{
    constexpr std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
    VersionBlobHandler handler(createMockVersionConfigs(blobNames));
    EXPECT_THAT(handler.getBlobIds(),
                ::testing::UnorderedElementsAreArray(blobNames));
}

} // namespace ipmi_flash
