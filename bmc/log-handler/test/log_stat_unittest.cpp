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

using ::testing::_;
using ::testing::Return;

namespace ipmi_flash
{

class LogStatBlobTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        h = std::make_unique<LogBlobHandler>(
            createMockLogConfigs(blobNames, &im, &tm));

        EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
        EXPECT_TRUE(h->open(0, blobs::read, "blob0"));

        blobs::BlobMeta meta;
        EXPECT_TRUE(h->stat(0, &meta));
        EXPECT_EQ(blobs::StateFlags::committing, meta.blobState);
    }

    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
};

TEST_F(LogStatBlobTest, CreateError)
{
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::failed));
    tm.at("blob0")->cb(*tm.at("blob0"));

    blobs::BlobMeta meta;
    EXPECT_TRUE(h->stat(0, &meta));
    EXPECT_EQ(blobs::StateFlags::commit_error, meta.blobState);
}

class LogStatSizeBlobTest :
    public LogStatBlobTest,
    public ::testing::WithParamInterface<std::vector<uint8_t>>
{};

TEST_P(LogStatSizeBlobTest, StatWithSize)
{
    const std::vector<uint8_t> data = GetParam();
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(0, ::testing::Ge(data.size())))
        .WillOnce(Return(data));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);
    tm.at("blob0")->cb(*tm.at("blob0"));

    blobs::BlobMeta meta;
    EXPECT_TRUE(h->stat(0, &meta));
    EXPECT_EQ(blobs::StateFlags::committed | blobs::StateFlags::open_read,
              meta.blobState);
    EXPECT_EQ(data.size(), meta.size);
}

const std::vector<std::vector<uint8_t>> data = {
    {},
    {0, 1, 2, 3, 4, 5, 6},
};

INSTANTIATE_TEST_SUITE_P(DifferentData, LogStatSizeBlobTest,
                         testing::ValuesIn(data));

} // namespace ipmi_flash
