// Copyright 2021 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "log_handler.hpp"
#include "log_mock.hpp"

#include <array>

#include <gtest/gtest.h>

namespace ipmi_flash
{

TEST(LogHandlerCanHandleTest, VerifyGoodInfoMap)
{
    constexpr std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
    LogBlobHandler handler(createMockLogConfigs(blobNames));
    for (const auto& blobName : blobNames)
    {
        EXPECT_TRUE(handler.canHandleBlob(blobName));
    }
}

TEST(LogHandlerEnumerateTest, VerifyGoodInfoMap)
{
    constexpr std::array blobNames{"blob0", "blob1", "blob2", "blob3"};
    LogBlobHandler handler(createMockLogConfigs(blobNames));
    EXPECT_THAT(handler.getBlobIds(),
                ::testing::UnorderedElementsAreArray(blobNames));
}

} // namespace ipmi_flash
