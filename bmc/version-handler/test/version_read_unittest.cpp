#include "version_handler.hpp"
#include "version_mock.hpp"

#include <memory>
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
        h = std::make_unique<VersionBlobHandler>(
            createMockVersionConfigs(blobNames, &im, &tm));
    }
    std::unique_ptr<blobs::GenericBlobInterface> h;
    std::vector<std::string> blobNames{"blob0", "blob1", "blob2", "blob3"};
    std::unordered_map<std::string, TriggerMock*> tm;
    std::unordered_map<std::string, ImageHandlerMock*> im;
    const std::uint16_t defaultSessionNumber{200};
    std::vector<uint8_t> vector1{0xDE, 0xAD, 0xBE, 0xEF,
                                 0xBA, 0xDF, 0xEE, 0x0D};
};

TEST_F(VersionReadBlobTest, VerifyValidRead)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_CALL(*tm.at("blob0"), status())
        .Times(2)
        .WillRepeatedly(Return(ActionStatus::success));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    /* file path gets bound to file_handler on creation so path parameter
     * doesn't actually matter
     */
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in))
        .Times(2)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(0, 10)).WillOnce(Return(vector1));
    EXPECT_CALL(*im.at("blob0"), read(2, 10)).WillOnce(Return(vector1));
    EXPECT_CALL(*im.at("blob0"), close()).Times(2);

    EXPECT_EQ(h->read(defaultSessionNumber, 0, 10), vector1);
    EXPECT_EQ(h->read(defaultSessionNumber, 2, 10), vector1);
}

TEST_F(VersionReadBlobTest, VerifyUnopenedReadFails)
{
    EXPECT_THAT(h->read(defaultSessionNumber, 0, 10), IsEmpty());
}

TEST_F(VersionReadBlobTest, VerifyTriggerFailureReadFails)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::failed));
    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THAT(h->read(defaultSessionNumber, 0, 10), IsEmpty());
}

TEST_F(VersionReadBlobTest, VerifyReadFailsOnFileReadFailure)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    /* file path gets bound to file_handler on creation so path parameter
     * doesn't actually matter
     */
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(true));
    EXPECT_CALL(*im.at("blob0"), read(_, _)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*im.at("blob0"), close()).Times(1);

    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THAT(h->read(defaultSessionNumber, 0, 10), IsEmpty());
}

TEST_F(VersionReadBlobTest, VerifyReadFailsOnFileOpenFailure)
{
    EXPECT_CALL(*tm.at("blob0"), trigger()).WillOnce(Return(true));
    /* first call to trigger status fails, second succeeds */
    EXPECT_CALL(*tm.at("blob0"), status())
        .WillOnce(Return(ActionStatus::success));
    /* file path gets bound to file_handler on creation so path parameter
     * doesn't actually matter
     */
    EXPECT_CALL(*im.at("blob0"), open(_, std::ios::in)).WillOnce(Return(false));

    EXPECT_TRUE(h->open(defaultSessionNumber, blobs::read, "blob0"));
    EXPECT_THAT(h->read(defaultSessionNumber, 0, 10), IsEmpty());
}

} // namespace ipmi_flash
