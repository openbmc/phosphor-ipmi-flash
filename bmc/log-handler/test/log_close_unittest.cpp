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

class LogCloseExpireBlobTest : public ::testing::Test
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
};

TEST_F(LogCloseExpireBlobTest, VerifyOpenThenClose)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifySingleAbort)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_TRUE(h->open(1, blobs::read, "blob0"));
    EXPECT_TRUE(h->close(0));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(1));
}

TEST_F(LogCloseExpireBlobTest, VerifyUnopenedBlobCloseFails)
{
    EXPECT_FALSE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifyDoubleCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
    EXPECT_FALSE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifyBadSessionNumberCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_FALSE(h->close(1));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifyRunningActionIsAborted)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

} // namespace ipmi_flash
