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

#include "general_systemd.hpp"
#include "log_handlers_builder.hpp"
#include "skip_action.hpp"

#include <nlohmann/json.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{
using ::testing::IsEmpty;

using json = nlohmann::json;

TEST(LogJsonTest, ValidConfigurationNoLogHandler)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seq",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute"
                    }
                 }
            }
         }]
    )"_json;
    auto h = LogHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(1));
    EXPECT_THAT(h[0].blobId, "/log/sink_seq");
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].handler == nullptr);
}

TEST(LogJsonTest, ValidConfigurationLogBlobName)
{
    auto j2 = R"(
        [{
            "blob" : "/log/sink_seq",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions": {
                    "open" : {
                    "type" : "systemd",
                    "unit" : "phosphor-ipmi-flash-log-sink-sequencer.target"
                    }
                 }
            }
         }]
    )"_json;
    auto h = LogHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(1));
    EXPECT_THAT(h[0].blobId, "/log/sink_seq");
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].handler == nullptr);
}

TEST(LogJsonTest, MissingHandlerType)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "log":{
                "handler": {
                   "path" : "/tmp/log_info"
                 },
                "actions": {
                  "open" : {
                  "type" : "systemd",
                  "unit" : "absolute"}
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(LogHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(LogJsonTest, BadBlobName)
{
    auto j2 = R"(
        [{
            "blob" : "/bad/image",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions": {
                  "open" : {
                  "type" : "systemd",
                  "unit" : "absolute"}
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(LogHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(LogJsonTest, MissingActions)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(LogHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(LogJsonTest, MissingOpenAction)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions": {}
            }
         }]
    )"_json;
    EXPECT_THAT(LogHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(LogJsonTest, OneInvalidTwoValidSucceeds)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seq0",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute"
                    }
                 }
            }
         },
         {
            "blob" : "/log/sink_seq1",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute"
                    }
                 }
            }
         },
         {
            "blob" : "/bad/sink_seq",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute"
                    }
                 }
            }
         }
         ]
    )"_json;
    auto h = LogHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(2));
    EXPECT_THAT(h[0].blobId, "/log/sink_seq0");
    EXPECT_THAT(h[1].blobId, "/log/sink_seq1");
}

TEST(LogJsonTest, BlobNameIsTooShort)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute"
                    }
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(LogHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(LogJsonTest, OpenSkipAction)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seqs",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "skip"
                    }
                 }
            }
         }]
    )"_json;
    auto h = LogHandlersBuilder().buildHandlerFromJson(j2);
    EXPECT_THAT(h, ::testing::SizeIs(1));
    EXPECT_TRUE(h[0].blobId == "/log/sink_seqs");
    ASSERT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].actions->onOpen == nullptr);
}

TEST(LogJsonTest, OpenActionsWithDifferentModes)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/blob1",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute",
                    "mode" : "replace-nope"
                    }
                 }
            }
         },
         {
            "blob" : "/flash/blob2",
            "log":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/log_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "systemd",
                    "unit" : "absolute",
                    "mode" : "replace-fake"
                    }
                 }
            }
         }
         ]
    )"_json;
    auto h = LogHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(2));

    EXPECT_FALSE(h[0].handler == nullptr);
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_THAT(h[0].blobId, "/log/blob1");
    auto onOpen0 = reinterpret_cast<SystemdNoFile*>(h[0].actions->onOpen.get());
    EXPECT_THAT(onOpen0->getMode(), "replace-nope");

    EXPECT_FALSE(h[1].handler == nullptr);
    EXPECT_FALSE(h[1].actions == nullptr);
    EXPECT_THAT(h[1].blobId, "/log/blob2");
    auto onOpen1 = reinterpret_cast<SystemdNoFile*>(h[1].actions->onOpen.get());
    EXPECT_THAT(onOpen1->getMode(), "replace-fake");
}
} // namespace
} // namespace ipmi_flash
