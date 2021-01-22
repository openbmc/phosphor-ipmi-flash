#include "version_handler.hpp"
#include "version_mock.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

using ::testing::Return;

namespace ipmi_flash
{

class VersionOpenBlobTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        h = std::make_unique<VersionBlobHandler>(
            createMockVersionConfigs(blobNames, &im, &tm));
    }

    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0", "blob1", "blob2", "blob3"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
    const std::uint16_t defaultSessionNumber{0};
};

TEST_F(VersionOpenBlobTest, VerifySingleBlobOpen)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
}

TEST_F(VersionOpenBlobTest, VerifyMultipleBlobOpens)
{
    for (const auto& [_, val] : tm)
    {
        /* set the expectation that every onOpen will be triggered */
        EXPECT_CALL(*val, trigger()).WillOnce(Return(true));
    }
    int i{defaultSessionNumber};
    for (const auto& blob : blobNames)
    {
        EXPECT_TRUE(h->open(i++, blobs::read, blob));
    }
}

TEST_F(VersionOpenBlobTest, VerifyOpenAfterClose)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));

    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(defaultSessionNumber));

    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
}

TEST_F(VersionOpenBlobTest, VerifyMultiOpenWorks)
{
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
    EXPECT_TRUE(h->open(1, blobs::read, "blob1"));
    EXPECT_TRUE(h->open(2, blobs::read, "blob1"));
}

TEST_F(VersionOpenBlobTest, VerifyFailedTriggerFails)
{
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(false));
    EXPECT_FALSE(h->open(0, blobs::read, "blob1"));
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
}

TEST_F(VersionOpenBlobTest, VerifyUnsupportedOpenFlagsFails)
{
    EXPECT_FALSE(h->open(0, blobs::write, "blob1"));
    EXPECT_CALL(*tm.at("blob1"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
}

} // namespace ipmi_flash
