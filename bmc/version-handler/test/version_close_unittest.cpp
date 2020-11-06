#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"
#include "version_handler.hpp"

#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>
using ::testing::_;
using ::testing::Return;
namespace ipmi_flash
{

class VersionCloseExpireBlobTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        VersionInfoMap vim;
        for (const auto& blobName : blobNames)
        {
            auto t = CreateTriggerMock();
            auto i = CreateImageMock();
            tm[blobName] = reinterpret_cast<TriggerMock*>(t.get());
            im[blobName] = reinterpret_cast<ImageHandlerMock*>(i.get());
            vim.try_emplace(
                blobName,
                VersionInfoPack(
                    blobName, std::make_unique<VersionActionPack>(std::move(t)),
                    std::move(i)));
        }
        h = VersionBlobHandler::create(std::move(vim));
        ASSERT_NE(h, nullptr);
        for (const auto& [key, val] : tm)
        {
            ON_CALL(*val, trigger()).WillByDefault(Return(true));
        }
    }
    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0", "blob1", "blob2", "blob3"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
};

TEST_F(VersionCloseExpireBlobTest, VerifyOpenThenClose)
{
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(0);
    EXPECT_TRUE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyUnopenedBlobCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), status()).Times(0);
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(0);
    EXPECT_FALSE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyDoubleCloseFails)
{
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(0);
    EXPECT_TRUE(h->close(0));
    EXPECT_FALSE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyBadSessionNumberCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::success));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(0);
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_FALSE(h->close(1));
    EXPECT_TRUE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyRunningActionIsAborted)
{
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::running));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_TRUE(h->close(0));
}

} // namespace ipmi_flash
