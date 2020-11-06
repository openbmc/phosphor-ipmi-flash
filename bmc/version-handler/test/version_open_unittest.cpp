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

class VersionOpenBlobTest : public ::testing::Test
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
            /* by default no action triggers expected to be called */
            EXPECT_CALL(*val, trigger()).Times(0);
        }
        for (const auto& [key, val] : im)
        {
            /* by default no image handler open is expected to be called */
            EXPECT_CALL(*val, open(_, _)).Times(0);
        }
    }
    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0", "blob1", "blob2", "blob3"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
};

TEST_F(VersionOpenBlobTest, VerifySingleBlobOpen)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).Times(1).WillOnce(Return(true));
    int i{0};
    EXPECT_TRUE(h->open(i++, blobs::read, "blob0"));
}

TEST_F(VersionOpenBlobTest, VerifyMultipleBlobOpens)
{
    for (const auto& [key, val] : tm)
    {
        /* set the expectation that every onOpen will be triggered */
        EXPECT_CALL(*val, trigger()).Times(1).WillOnce(Return(true));
    }
    int i{0};
    for (const auto& blob : blobNames)
    {
        EXPECT_TRUE(h->open(i++, blobs::read, blob));
    }
}

TEST_F(VersionOpenBlobTest, VerifyOpenAfterClose)
{
    EXPECT_CALL(*tm.at("blob0"), trigger())
        .Times(2)
        .WillRepeatedly(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
    EXPECT_TRUE(h->close(0));
    EXPECT_TRUE(h->open(0, blobs::read, "blob0"));
}

TEST_F(VersionOpenBlobTest, VerifyDuplicateSessionNumberFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*tm.at("blob1"), trigger()).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
    /* the duplicate session number of 0
     *  should cause a failure for the open of a different blob
     */
    EXPECT_FALSE(h->open(0, blobs::read, "blob0"));
    /* open after fail due to seq number works */
    EXPECT_TRUE(h->open(1, blobs::read, "blob0"));
}

TEST_F(VersionOpenBlobTest, VerifyDoubleOpenFails)
{
    EXPECT_CALL(*tm.at("blob1"), trigger())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
    EXPECT_FALSE(h->open(2, blobs::read, "blob1"));
}

TEST_F(VersionOpenBlobTest, VerifyFailedTriggerFails)
{
    EXPECT_CALL(*tm.at("blob1"), trigger())
        .Times(2)
        .WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_FALSE(h->open(0, blobs::read, "blob1"));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));

}

TEST_F(VersionOpenBlobTest, VerifyUnsupportedOpenFlagsFails)
{
    EXPECT_CALL(*tm.at("blob1"), trigger()).Times(1).WillOnce(Return(true));
    EXPECT_FALSE(h->open(0, blobs::write, "blob1"));
    EXPECT_TRUE(h->open(0, blobs::read, "blob1"));
}

} // namespace ipmi_flash
