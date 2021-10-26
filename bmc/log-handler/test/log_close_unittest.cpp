#include "log_handler.hpp"
#include "log_mock.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

using ::testing::Return;

namespace ipmi_flash
{

class LogCloseExpireBlobTest : public ::testing::Test
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
};

TEST_F(LogCloseExpireBlobTest, VerifyOpenThenClose)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifySingleAbort)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_TRUE(h->open(1, blobs::read, "blob0"));
    EXPECT_TRUE(h->close(0));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(1));
}

TEST_F(LogCloseExpireBlobTest, VerifyUnopenedBlobCloseFails)
{
    EXPECT_FALSE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifyDoubleCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
    EXPECT_FALSE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifyBadSessionNumberCloseFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_FALSE(h->close(1));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

TEST_F(LogCloseExpireBlobTest, VerifyRunningActionIsAborted)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_CALL(*tm.at("blob0"), abort()).Times(1);
    EXPECT_TRUE(h->close(0));
}

} // namespace ipmi_flash
