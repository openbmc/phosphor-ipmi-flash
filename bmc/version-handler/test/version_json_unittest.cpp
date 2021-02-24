#include "general_systemd.hpp"
#include "skip_action.hpp"
#include "version_handlers_builder.hpp"

#include <nlohmann/json.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{
using ::testing::IsEmpty;
using testing::Not;
using testing::SizeIs;
using testing::internal::CaptureStderr;
using testing::internal::GetCapturedStderr;

using json = nlohmann::json;

TEST(VersionJsonTest, FlashBlobNoVersionOK)
{
    CaptureStderr();
    auto j2 = R"(
        [{
            "blob" : "/flash/sink_seq0"
         },
         {
            "blob" : "/version/sink_seq1",
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
         },
         {
            "blob" : "/flash/sink_seq2"
         },
         {
            "blob" : "/version/sink_seq3",
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
         }

         ]
    )"_json;
    auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    ASSERT_THAT(h, SizeIs(2));
    EXPECT_THAT(h[0].blobId, "/version/sink_seq1");
    EXPECT_THAT(h[1].blobId, "/version/sink_seq3");
    EXPECT_THAT(GetCapturedStderr(), IsEmpty());
}

TEST(VersionJsonTest, VersionBlobNoVersionPrintsWarning)
{
    CaptureStderr();
    auto j2 = R"(
        [{
            "blob" : "/version/sink_seq"
         },
         {
            "blob" : "/version/sink_seq3",
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
         }
         ]
    )"_json;
    auto h = VersionHandlersBuilder().buildHandlerFromJson(j2);
    EXPECT_THAT(h, SizeIs(1));
    EXPECT_THAT(h[0].blobId, "/version/sink_seq3");
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, ValidConfigurationNoVersionHandler)
{
    CaptureStderr();
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
    ASSERT_THAT(h, SizeIs(1));
    EXPECT_THAT(h[0].blobId, "/version/sink_seq");
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].handler == nullptr);
    EXPECT_THAT(GetCapturedStderr(), IsEmpty());
}

TEST(VersionJsonTest, ValidConfigurationVersionBlobName)
{
    CaptureStderr();
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
    ASSERT_THAT(h, SizeIs(1));
    EXPECT_THAT(h[0].blobId, "/version/sink_seq");
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].handler == nullptr);
    EXPECT_THAT(GetCapturedStderr(), IsEmpty());
}

TEST(VersionJsonTest, MissingHandlerType)
{
    CaptureStderr();
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
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, BadBlobName)
{
    CaptureStderr();
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
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, MissingActions)
{
    CaptureStderr();
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
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, MissingOpenAction)
{
    CaptureStderr();
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
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, OneInvalidTwoValidSucceeds)
{
    CaptureStderr();
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
    ASSERT_THAT(h, SizeIs(2));
    EXPECT_THAT(h[0].blobId, "/version/sink_seq0");
    EXPECT_THAT(h[1].blobId, "/version/sink_seq1");
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, BlobNameIsTooShort)
{
    CaptureStderr();
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
    EXPECT_THAT(GetCapturedStderr(), Not(IsEmpty()));
}

TEST(VersionJsonTest, OpenSkipAction)
{
    CaptureStderr();
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
    EXPECT_THAT(h, SizeIs(1));
    EXPECT_TRUE(h[0].blobId == "/version/sink_seqs");
    ASSERT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].actions->onOpen == nullptr);
    EXPECT_THAT(GetCapturedStderr(), IsEmpty());
}

TEST(VersionJsonTest, OpenActionsWithDifferentModes)
{
    CaptureStderr();
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
    ASSERT_THAT(h, SizeIs(2));

    EXPECT_FALSE(h[0].handler == nullptr);
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_THAT(h[0].blobId, "/version/blob1");
    auto onOpen0 = reinterpret_cast<SystemdNoFile*>(h[0].actions->onOpen.get());
    EXPECT_THAT(onOpen0->getMode(), "replace-nope");

    EXPECT_FALSE(h[1].handler == nullptr);
    EXPECT_FALSE(h[1].actions == nullptr);
    EXPECT_THAT(h[1].blobId, "/version/blob2");
    auto onOpen1 = reinterpret_cast<SystemdNoFile*>(h[1].actions->onOpen.get());
    EXPECT_THAT(onOpen1->getMode(), "replace-fake");
    EXPECT_THAT(GetCapturedStderr(), IsEmpty());
}
} // namespace
} // namespace ipmi_flash
