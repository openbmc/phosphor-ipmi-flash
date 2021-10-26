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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

using ::testing::Return;

namespace ipmi_flash
{

class LogOpenBlobTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        h = std::make_unique<LogBlobHandler>(
            createMockLogConfigs(blobNames, &im, &tm));
    }

    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0", "blob1", "blob2", "blob3"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
    const std::uint16_t defaultSessionNumber{0};
};

TEST_F(LogOpenBlobTest, VerifySingleBlobOpen)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
}

TEST_F(LogOpenBlobTest, VerifyMultipleBlobOpens)
{
    for (const auto& [_, val] : tm)
    {
        /* set the expectation that every onOpen will be triggered */
        EXPECT_CALL(*val, trigger()).WillOnce(Return(true));
    }
    int i{defaultSessionNumber};
    for (const auto& blob : blobNames)
    {
        EXPECT_TRUE(h->open(i++, blobs::read, blob));
    }
}

TEST_F(LogOpenBlobTest, VerifyOpenAfterClose)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));

    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(defaultSessionNumber));

    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
}

TEST_F(LogOpenBlobTest, VerifyMultiOpenWorks)
{
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
    EXPECT_TRUE(h->open(1, blobs::read, "blob1"));
    EXPECT_TRUE(h->open(2, blobs::read, "blob1"));
}

TEST_F(LogOpenBlobTest, VerifyFailedTriggerFails)
{
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(false));
    EXPECT_FALSE(h->open(0, blobs::read, "blob1"));
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
}

TEST_F(LogOpenBlobTest, VerifyUnsupportedOpenFlagsFails)
{
    EXPECT_FALSE(h->open(0, blobs::write, "blob1"));
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
}

} // namespace ipmi_flash
