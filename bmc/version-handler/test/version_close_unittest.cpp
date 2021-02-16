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

class VersionCloseExpireBlobTest : public ::testing::Test
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
};

TEST_F(VersionCloseExpireBlobTest, VerifyOpenThenClose)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifySingleAbort)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_TRUE(h->open(1, blobs::read, "blob0"));
    EXPECT_TRUE(h->close(0));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(1));
}

TEST_F(VersionCloseExpireBlobTest, VerifyUnopenedBlobCloseFails)
{
    EXPECT_FALSE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyDoubleCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
    EXPECT_FALSE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyBadSessionNumberCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_FALSE(h->close(1));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

TEST_F(VersionCloseExpireBlobTest, VerifyRunningActionIsAborted)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

} // namespace ipmi_flash
