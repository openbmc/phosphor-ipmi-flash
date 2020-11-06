#include "version_handlers_builder.hpp"
#include "general_systemd.hpp"
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

TEST(VersionJsonTest, ValidConfigurationNoFirmwareHandler)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seq",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
  auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(1));
    EXPECT_THAT(h[0].blobId,"/version/sink_seq");
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].handler == nullptr);
}

TEST(VersionJsonTest, ValidConfigurationVersionBlobName)
{
    auto j2 = R"(
        [{
            "blob" : "/version/sink_seq",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
                 },
                "actions": {
                    "open" : {
                    "type" : "systemd",
                    "unit" : "phosphor-ipmi-flash-version-sink-sequencer.target"
                    }
                 }
            }
         }]
    )"_json;
  auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(1));
    EXPECT_THAT(h[0].blobId,"/version/sink_seq");
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].handler == nullptr);
}

TEST(VersionJsonTest, MissingHandlerType)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "version":{
                "handler": {
                   "path" : "/tmp/version_info"
                 },
                "actions": {
                  "open" : {
                  "type" : "systemd",
                  "unit" : "absolute"}
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(VersionHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(VersionJsonTest, BadBlobName)
{
    auto j2 = R"(
        [{
            "blob" : "/bad/image",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
                 },
                "actions": {
                  "open" : {
                  "type" : "systemd",
                  "unit" : "absolute"}
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(VersionHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(VersionJsonTest, MissingActions)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
                 }
            }
         }]
    )"_json;
    EXPECT_THAT(VersionHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(VersionJsonTest, MissingOpenAction)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
                 },
                "actions": {}
            }
         }]
    )"_json;
    EXPECT_THAT(VersionHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(VersionJsonTest, OneInvalidTwoValidSucceeds)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seq0",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
            "blob" : "/version/sink_seq1",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
  auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(2));
    EXPECT_THAT(h[0].blobId,"/version/sink_seq0");
    EXPECT_THAT(h[1].blobId,"/version/sink_seq1");
}

TEST(VersionJsonTest, BlobNameIsTooShort)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
  EXPECT_THAT(VersionHandlersBuilder().buildHandlerFromJson(j2), IsEmpty());
}

TEST(VersionJsonTest, OpenSkipAction)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seqs",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
                 },
                "actions":{
                    "open" :{
                    "type" : "skip"
                    }
                 }
            }
         }]
    )"_json;
    auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    EXPECT_THAT(h, ::testing::SizeIs(1));
    EXPECT_TRUE(h[0].blobId == "/version/sink_seqs");
    ASSERT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].actions->onOpen == nullptr);
}

TEST(VersionJsonTest, OpenActionsWithDifferentModes)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/blob1",
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
            "version":{
                "handler": {
                   "type" : "file",
                   "path" : "/tmp/version_info"
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
  auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, ::testing::SizeIs(2));

    EXPECT_FALSE(h[0].handler == nullptr);
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_THAT(h[0].blobId,"/version/blob1");
    auto onOpen0 = reinterpret_cast<SystemdNoFile*>(h[0].actions->onOpen.get());
    EXPECT_THAT(onOpen0->getMode(), "replace-nope");

    EXPECT_FALSE(h[1].handler == nullptr);
    EXPECT_FALSE(h[1].actions == nullptr);
    EXPECT_THAT(h[1].blobId,"/version/blob2");
    auto onOpen1 = reinterpret_cast<SystemdNoFile*>(h[1].actions->onOpen.get());
    EXPECT_THAT(onOpen1->getMode(), "replace-fake");
}
} // namespace
} // namespace ipmi_flash
