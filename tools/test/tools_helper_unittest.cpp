#include "helper.hpp"
#include "status.hpp"

#include <ipmiblob/test/blob_interface_mock.hpp>

#include <cstdint>

#include <gtest/gtest.h>

namespace host_tool
{
using ::testing::Return;
using ::testing::TypedEq;

class HelperTest : public ::testing::Test
{
  protected:
    ipmiblob::BlobInterfaceMock blobMock;
    std::uint16_t session = 0xbeef;
};

TEST_F(HelperTest, PollStatusReturnsAfterSuccess)
{
    ipmiblob::StatResponse verificationResponse = {};
    /* the other details of the response are ignored, and should be. */
    verificationResponse.metadata.push_back(
        static_cast<std::uint8_t>(ipmi_flash::ActionStatus::success));

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(verificationResponse));

    EXPECT_TRUE(pollStatus(session, &blobMock));
}

TEST_F(HelperTest, PollStatusReturnsAfterFailure)
{
    ipmiblob::StatResponse verificationResponse = {};
    /* the other details of the response are ignored, and should be. */
    verificationResponse.metadata.push_back(
        static_cast<std::uint8_t>(ipmi_flash::ActionStatus::failed));

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(verificationResponse));

    EXPECT_FALSE(pollStatus(session, &blobMock));
}

TEST_F(HelperTest, MemcpyAlignedOneByte)
{
    const char source = 'a';
    char destination;

    EXPECT_EQ(&destination,
              memcpyAligned(&destination, &source, sizeof(source)));
    EXPECT_EQ(destination, source);
}

TEST_F(HelperTest, MemcpyAlignedMultiByte)
{
    const char source[14] = "abcdefghijklm";
    char destination[14] = "xxxxxxxxxxxxx";

    EXPECT_EQ(&destination,
              memcpyAligned(&destination, &source, sizeof(source)));
    EXPECT_EQ(0, memcmp(&destination, &source, sizeof(source)));
}

} // namespace host_tool
