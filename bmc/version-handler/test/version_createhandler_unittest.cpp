#include "version_handler.hpp"
#include "version_mock.hpp"

#include <array>
#include <utility>

#include <gtest/gtest.h>

namespace ipmi_flash
{

TEST(VersionHandlerCanHandleTest, VerifyGoodInfoMapPasses)
{
    constexpr std::array blobs{"blob0", "blob1"};
    VersionBlobHandler handler(createMockVersionConfigs(blobs));
    EXPECT_THAT(handler.getBlobIds(),
                testing::UnorderedElementsAreArray(blobs));
}

TEST(VersionHandlerCanHandleTest, VerifyDuplicatesIgnored)
{
    constexpr std::array blobs{"blob0"};
    auto configs = createMockVersionConfigs(blobs);
    configs.push_back(createMockVersionConfig(blobs[0]));
    VersionBlobHandler handler(std::move(configs));
    EXPECT_THAT(handler.getBlobIds(),
                testing::UnorderedElementsAreArray(blobs));
}

} // namespace ipmi_flash
