#include "buildjson.hpp"

#include <nlohmann/json.hpp>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{
using json = nlohmann::json;

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
    EXPECT_EQ(h.blobId, "/flash/image");
    EXPECT_FALSE(h.handler == nullptr);
    EXPECT_FALSE(h.actions == nullptr);
    EXPECT_FALSE(h.actions->preparation == nullptr);
    EXPECT_FALSE(h.actions->verification == nullptr);
    EXPECT_FALSE(h.actions->update == nullptr);
}

} // namespace
} // namespace ipmi_flash
