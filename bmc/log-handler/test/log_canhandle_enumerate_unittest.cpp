#include <gtest/gtest.h>

#include <array>

#include "log_handler.hpp"
#include "log_mock.hpp"

namespace ipmi_flash {

TEST(LogHandlerCanHandleTest, VerifyGoodInfoMap) {
  constexpr std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
  LogBlobHandler handler(createMockLogConfigs(blobNames));
  for (const auto& blobName : blobNames) {
    EXPECT_TRUE(handler.canHandleBlob(blobName));
  }
}

TEST(LogHandlerEnumerateTest, VerifyGoodInfoMap) {
  constexpr std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
  LogBlobHandler handler(createMockLogConfigs(blobNames));
  EXPECT_THAT(handler.getBlobIds(),
              ::testing::UnorderedElementsAreArray(blobNames));
}

}  // namespace ipmi_flash
