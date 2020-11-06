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
using ::testing::IsEmpty;
using ::testing::Return;
namespace ipmi_flash
{

class VersionReadBlobTest : public ::testing::Test
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
    std::vector<uint8_t> vector1{0xDE, 0xAD, 0xBE, 0xEF,
                                 0xBA, 0xDF, 0xEE, 0x0D};
};

TEST_F(VersionReadBlobTest, VerifyValidRead)
{
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(2)
        .WillRepeatedly(Return(ActionStatus::success));
    EXPECT_TRUE(h->open(200, blobs::read, "blob0"));
    /* file path gets bound to file_handler on creation so path parameter
     * doesn't actually matter
     */
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in))
        .Times(2)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(0, 10)).WillOnce(Return(vector1));
    EXPECT_CALL(*im.at("blob0"), read(2, 10)).WillOnce(Return(vector1));
    EXPECT_CALL(*im.at("blob0"), close()).Times(2);

    EXPECT_EQ(h->read(200, 0, 10), vector1);
    EXPECT_EQ(h->read(200, 2, 10), vector1);
}

TEST_F(VersionReadBlobTest, VerifyUnopenedReadFails)
{
    EXPECT_CALL(*tm.at("blob0"), status()).Times(0);
    EXPECT_CALL(*im.at("blob0"), open(_, _)).Times(0);
    EXPECT_CALL(*im.at("blob0"), read(_, _)).Times(0);

    EXPECT_THAT(h->read(200, 0, 10), IsEmpty());
}

TEST_F(VersionReadBlobTest, VerifyTriggerFailureReadFails)
{
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::failed));
    EXPECT_CALL(*im.at("blob0"), open(_, _)).Times(0);
    EXPECT_TRUE(h->open(200, blobs::read, "blob0"));
    EXPECT_THAT(h->read(200, 0, 10), IsEmpty());
}

TEST_F(VersionReadBlobTest, VerifyReadFailsOnFileReadFailure)
{
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::success));
    /* file path gets bound to file_handler on creation so path parameter
     * doesn't actually matter
     */
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(_, _))
        .Times(1)
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);

    EXPECT_TRUE(h->open(200, blobs::read, "blob0"));
    EXPECT_THAT(h->read(200, 0, 10), IsEmpty());
}

TEST_F(VersionReadBlobTest, VerifyReadFailsOnFileOpenFailure)
{
    /* first call to trigger status fails, second succeeds */
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(1)
        .WillOnce(Return(ActionStatus::success));
    /* file path gets bound to file_handler on creation so path parameter
     * doesn't actually matter
     */
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in))
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_TRUE(h->open(200, blobs::read, "blob0"));
    EXPECT_THAT(h->read(200, 0, 10), IsEmpty());
}

} // namespace ipmi_flash
