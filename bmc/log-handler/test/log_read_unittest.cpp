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
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::DoAll;
using ::testing::ElementsAreArray;
using ::testing::Ge;
using ::testing::IsEmpty;
using ::testing::Return;

namespace ipmi_flash
{

class LogReadBlobTest : public ::testing::Test
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
    const std::uint16_t defaultSessionNumber{200};
    std::vector<uint8_t> vector1{0xDE, 0xAD, 0xBE, 0xEF,
                                 0xBA, 0xDF, 0xEE, 0x0D};
    std::vector<uint8_t> vector2{0xCE, 0xAD, 0xDE, 0xFF};
};

TEST_F(LogReadBlobTest, VerifyValidRead)
{
    testing::InSequence seq;
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(DoAll([&]() {
        tm.at("blob0")->cb(*tm.at("blob0"));
    }, Return(true)));
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(0, Ge(vector1.size())))
        .WillOnce(Return(vector1));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));

    std::basic_string_view<uint8_t> vectorS(vector1.data(), vector1.size());
    EXPECT_THAT(h->read(defaultSessionNumber, 0, 7),
                ElementsAreArray(vectorS.substr(0, 7)));
    EXPECT_THAT(h->read(defaultSessionNumber, 2, 10),
                ElementsAreArray(vectorS.substr(2, 6)));
    EXPECT_THAT(h->read(defaultSessionNumber, 10, 0), IsEmpty());
}

TEST_F(LogReadBlobTest, VerifyMultipleSession)
{
    testing::InSequence seq;
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_TRUE(h->open(1, blobs::read, "blob0"));

    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(0, Ge(vector1.size())))
        .WillOnce(Return(vector1));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);
    tm.at("blob0")->cb(*tm.at("blob0"));

    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(2, blobs::read, "blob0"));

    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(0, Ge(vector2.size())))
        .WillOnce(Return(vector2));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);
    tm.at("blob0")->cb(*tm.at("blob0"));

    EXPECT_THAT(h->read(0, 0, 10), ElementsAreArray(vector1));
    EXPECT_THAT(h->read(1, 0, 10), ElementsAreArray(vector1));
    EXPECT_THAT(h->read(2, 0, 10), ElementsAreArray(vector2));
}

TEST_F(LogReadBlobTest, VerifyReadEarlyFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));

    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THROW(h->read(defaultSessionNumber, 0, 10), std::runtime_error);
}

TEST_F(LogReadBlobTest, VerifyTriggerFailureReadFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(DoAll([&]() {
        tm.at("blob0")->cb(*tm.at("blob0"));
    }, Return(true)));
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::failed));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THROW(h->read(defaultSessionNumber, 0, 10), std::runtime_error);
}

TEST_F(LogReadBlobTest, VerifyReadFailsOnFileOpenFailure)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(DoAll([&]() {
        tm.at("blob0")->cb(*tm.at("blob0"));
    }, Return(true)));
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(false));

    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THROW(h->read(defaultSessionNumber, 0, 10), std::runtime_error);
}

TEST_F(LogReadBlobTest, VerifyReadFailsOnFileReadFailure)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(DoAll([&]() {
        tm.at("blob0")->cb(*tm.at("blob0"));
    }, Return(true)));
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(_, _)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);

    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THROW(h->read(defaultSessionNumber, 0, 10), std::runtime_error);
}

} // namespace ipmi_flash
