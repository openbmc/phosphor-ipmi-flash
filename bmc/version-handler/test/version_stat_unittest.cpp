#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"
#include "version_handler.hpp"
#include "version_mock.hpp"

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>
using ::testing::_;
using ::testing::IsEmpty;
using ::testing::Return;
namespace ipmi_flash
{

class VersionStatTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        h = std::make_unique<VersionBlobHandler>(
            createMockVersionConfigs(blobNames, &im, &tm));
        for (const auto& [_, val] : tm)
        {
            ON_CALL(*val, trigger()).WillByDefault(Return(true));
        }
    }
    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
};

TEST_F(VersionStatTest, VerifyStatClosedBlob)
{
    constexpr std::uint32_t size{34};
    EXPECT_CALL(*im.at("blob0"), getSize()).Times(1).WillOnce(Return(size));
    blobs::BlobMeta meta;
    EXPECT_TRUE(h->stat("blob0", &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = 0,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::unknown)}}));
}

TEST_F(VersionStatTest, VerifyStatOpenedBlob)
{
    constexpr std::uint32_t size{87};
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(4)
        .WillOnce(Return(ActionStatus::running))
        .WillOnce(Return(ActionStatus::success))
        .WillOnce(Return(ActionStatus::failed))
        .WillOnce(Return(ActionStatus::unknown));

    EXPECT_CALL(*im.at("blob0"), getSize())
        .Times(4)
        .WillRepeatedly(Return(size));

    ASSERT_TRUE(h->open(200, blobs::read, "blob0"));
    blobs::BlobMeta meta;
    EXPECT_TRUE(h->stat("blob0", &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = blobs::StateFlags::open_read |
                                                  blobs::StateFlags::committing,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::running)}}));
    EXPECT_TRUE(h->stat("blob0", &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = blobs::StateFlags::open_read |
                                                  blobs::StateFlags::committed,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::success)}}));
    EXPECT_TRUE(h->stat("blob0", &meta));
    EXPECT_EQ(meta,
              blobs::BlobMeta({.blobState = blobs::StateFlags::open_read |
                                            blobs::StateFlags::commit_error,
                               .size = size,
                               .metadata = {static_cast<std::uint8_t>(
                                   ActionStatus::failed)}}));
    EXPECT_TRUE(h->stat("blob0", &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = blobs::StateFlags::open_read,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::unknown)}}));
}

TEST_F(VersionStatTest, VerifySessionStat)
{
    constexpr std::uint32_t size{87};
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(4)
        .WillOnce(Return(ActionStatus::running))
        .WillOnce(Return(ActionStatus::success))
        .WillOnce(Return(ActionStatus::failed))
        .WillOnce(Return(ActionStatus::unknown));

    EXPECT_CALL(*im.at("blob0"), getSize())
        .Times(4)
        .WillRepeatedly(Return(size));

    ASSERT_TRUE(h->open(200, blobs::read, "blob0"));
    blobs::BlobMeta meta;
    EXPECT_TRUE(h->stat(200, &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = blobs::StateFlags::open_read |
                                                  blobs::StateFlags::committing,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::running)}}));
    EXPECT_TRUE(h->stat(200, &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = blobs::StateFlags::open_read |
                                                  blobs::StateFlags::committed,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::success)}}));
    EXPECT_TRUE(h->stat(200, &meta));
    EXPECT_EQ(meta,
              blobs::BlobMeta({.blobState = blobs::StateFlags::open_read |
                                            blobs::StateFlags::commit_error,
                               .size = size,
                               .metadata = {static_cast<std::uint8_t>(
                                   ActionStatus::failed)}}));
    EXPECT_TRUE(h->stat(200, &meta));
    EXPECT_EQ(meta, blobs::BlobMeta({.blobState = blobs::StateFlags::open_read,
                                     .size = size,
                                     .metadata = {static_cast<std::uint8_t>(
                                         ActionStatus::unknown)}}));
}

TEST_F(VersionStatTest, VerifyBadSessionStat)
{
    EXPECT_CALL(*tm.at("blob0"), status()).Times(0);
    EXPECT_CALL(*im.at("blob0"), getSize()).Times(0);
    ASSERT_TRUE(h->open(200, blobs::read, "blob0"));
    blobs::BlobMeta meta;
    EXPECT_FALSE(h->stat(201, &meta));
}
} // namespace ipmi_flash
