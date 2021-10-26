#include "log_handler.hpp"
#include "log_mock.hpp"

#include <array>
#include <utility>

#include <gtest/gtest.h>

namespace ipmi_flash
{

TEST(LogHandlerCanHandleTest, VerifyGoodInfoMapPasses)
{
    constexpr std::array blobs{"blob0", "blob1"};
    LogBlobHandler handler(createMockLogConfigs(blobs));
    EXPECT_THAT(handler.getBlobIds(),
                testing::UnorderedElementsAreArray(blobs));
}

TEST(LogHandlerCanHandleTest, VerifyDuplicatesIgnored)
{
    constexpr std::array blobs{"blob0"};
    auto configs = createMockLogConfigs(blobs);
    configs.push_back(createMockLogConfig(blobs[0]));
    LogBlobHandler handler(std::move(configs));
    EXPECT_THAT(handler.getBlobIds(),
                testing::UnorderedElementsAreArray(blobs));
}

} // namespace ipmi_flash
