#include "buildjson.hpp"

#include <nlohmann/json.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{
using ::testing::IsEmpty;

using json = nlohmann::json;

TEST(FirmwareJsonTest, MissingHandler)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "actions" : {
                "preparation" : {
                    "type" : "systemdPrepare",
                    "unit" : "phosphor-ipmi-flash-bmc-prepare.target"
                },
                "verification" : {
                    "type" : "fileSystemdVerify",
                    "unit" : "phosphor-ipmi-flash-bmc-verify.target",
                    "path" : "/tmp/bmc.verify"
                },
                "update" : {
                    "type" : "reboot"
                }
            }
        }]
    )"_json;

    EXPECT_THAT(buildHandlerFromJson(j2), IsEmpty());
}

TEST(FirmwareJsonTest, MissingActions)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "handler" : {
                "type" : "file",
                "path" : "/run/initramfs/bmc-image"
            }
        }]
    )"_json;

    EXPECT_THAT(buildHandlerFromJson(j2), IsEmpty());
}

TEST(FirmwareJsonTest, MissingActionPreparation)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "handler" : {
                "type" : "file",
                "path" : "/run/initramfs/bmc-image"
            },
            "actions" : {
                "verification" : {
                    "type" : "fileSystemdVerify",
                    "unit" : "phosphor-ipmi-flash-bmc-verify.target",
                    "path" : "/tmp/bmc.verify"
                },
                "update" : {
                    "type" : "reboot"
                }
            }
        }]
    )"_json;

    EXPECT_THAT(buildHandlerFromJson(j2), IsEmpty());
}

TEST(FirmwareJsonTest, MissingActionVerification)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "handler" : {
                "type" : "file",
                "path" : "/run/initramfs/bmc-image"
            },
            "actions" : {
                "preparation" : {
                    "type" : "systemdPrepare",
                    "unit" : "phosphor-ipmi-flash-bmc-prepare.target"
                },
                "update" : {
                    "type" : "reboot"
                }
            }
        }]
    )"_json;

    EXPECT_THAT(buildHandlerFromJson(j2), IsEmpty());
}

TEST(FirmwareJsonTest, MissingActionUpdate)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "handler" : {
                "type" : "file",
                "path" : "/run/initramfs/bmc-image"
            },
            "actions" : {
                "preparation" : {
                    "type" : "systemdPrepare",
                    "unit" : "phosphor-ipmi-flash-bmc-prepare.target"
                },
                "verification" : {
                    "type" : "fileSystemdVerify",
                    "unit" : "phosphor-ipmi-flash-bmc-verify.target",
                    "path" : "/tmp/bmc.verify"
                }
            }
        }]
    )"_json;

    EXPECT_THAT(buildHandlerFromJson(j2), IsEmpty());
}

TEST(FirmwareJsonTest, TwoConfigsOneInvalidReturnsValid)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "actions" : {
                "preparation" : {
                    "type" : "systemdPrepare",
                    "unit" : "phosphor-ipmi-flash-bmc-prepare.target"
                },
                "verification" : {
                    "type" : "fileSystemdVerify",
                    "unit" : "phosphor-ipmi-flash-bmc-verify.target",
                    "path" : "/tmp/bmc.verify"
                },
                "update" : {
                    "type" : "reboot"
                }
            }
         },
         {
            "blob" : "/flash/image2",
            "handler" : {
                "type" : "file",
                "path" : "/run/initramfs/bmc-image"
            },
            "actions" : {
                "preparation" : {
                    "type" : "systemdPrepare",
                    "unit" : "phosphor-ipmi-flash-bmc-prepare.target"
                },
                "verification" : {
                    "type" : "fileSystemdVerify",
                    "unit" : "phosphor-ipmi-flash-bmc-verify.target",
                    "path" : "/tmp/bmc.verify"
                },
                "update" : {
                    "type" : "reboot"
                }
            }
        }]
    )"_json;

    auto h = buildHandlerFromJson(j2);
    EXPECT_EQ(h[0].blobId, "/flash/image2");
    EXPECT_EQ(h.size(), 1);
}

/*
 * TODO: It may be worth individually using builders per type, and testing
 * those.
 * TODO: Only allow unique handler blob paths.
 */

TEST(FirmwareJsonTest, VerifySystemd)
{
    auto j2 = R"(
        [{
            "blob" : "/flash/image",
            "handler" : {
                "type" : "file",
                "path" : "/run/initramfs/bmc-image"
            },
            "actions" : {
                "preparation" : {
                    "type" : "systemdPrepare",
                    "unit" : "phosphor-ipmi-flash-bmc-prepare.target"
                },
                "verification" : {
                    "type" : "fileSystemdVerify",
                    "unit" : "phosphor-ipmi-flash-bmc-verify.target",
                    "path" : "/tmp/bmc.verify"
                },
                "update" : {
                    "type" : "reboot"
                }
            }
         }]
    )"_json;

    auto h = buildHandlerFromJson(j2);
    EXPECT_EQ(h[0].blobId, "/flash/image");
    EXPECT_FALSE(h[0].handler == nullptr);
    EXPECT_FALSE(h[0].actions == nullptr);
    EXPECT_FALSE(h[0].actions->preparation == nullptr);
    EXPECT_FALSE(h[0].actions->verification == nullptr);
    EXPECT_FALSE(h[0].actions->update == nullptr);
}

} // namespace
} // namespace ipmi_flash
