#include "helper.hpp"
#include "status.hpp"
#include "tool_errors.hpp"

#include <blobs-ipmid/blobs.hpp>
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

    EXPECT_NO_THROW(pollStatus(session, &blobMock));
}

TEST_F(HelperTest, PollStatusReturnsAfterFailure)
{
    ipmiblob::StatResponse verificationResponse = {};
    /* the other details of the response are ignored, and should be. */
    verificationResponse.metadata.push_back(
        static_cast<std::uint8_t>(ipmi_flash::ActionStatus::failed));

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(verificationResponse));

    EXPECT_THROW(pollStatus(session, &blobMock), ToolException);
}

TEST_F(HelperTest, PollReadReadyReturnsAfterSuccess)
{
    ipmiblob::StatResponse blobResponse = {};
    /* the other details of the response are ignored, and should be. */
    blobResponse.blob_state =
        blobs::StateFlags::open_read | blobs::StateFlags::committed;

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(blobResponse));

    EXPECT_NO_THROW(pollReadReady(session, &blobMock));
}

TEST_F(HelperTest, PollReadReadyReturnsAfterFailure)
{
    ipmiblob::StatResponse blobResponse = {};
    /* the other details of the response are ignored, and should be. */
    blobResponse.blob_state = blobs::StateFlags::commit_error;

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(blobResponse));

    EXPECT_THROW(pollReadReady(session, &blobMock), ToolException);
}

TEST_F(HelperTest, PollReadReadyReturnsAfterRetrySuccess)
{
    ipmiblob::StatResponse blobResponseRunning = {};
    /* the other details of the response are ignored, and should be. */
    blobResponseRunning.blob_state = blobs::StateFlags::committing;

    ipmiblob::StatResponse blobResponseReadReady = {};
    /* the other details of the response are ignored, and should be. */
    blobResponseReadReady.blob_state = blobs::StateFlags::open_read;

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(blobResponseRunning))
        .WillOnce(Return(blobResponseReadReady));

    EXPECT_NO_THROW(pollReadReady(session, &blobMock));
}

TEST_F(HelperTest, PollReadReadyReturnsAfterRetryFailure)
{
    ipmiblob::StatResponse blobResponseRunning = {};
    /* the other details of the response are ignored, and should be. */
    blobResponseRunning.blob_state = blobs::StateFlags::committing;

    ipmiblob::StatResponse blobResponseError = {};
    /* the other details of the response are ignored, and should be. */
    blobResponseError.blob_state = blobs::StateFlags::commit_error;

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(blobResponseRunning))
        .WillOnce(Return(blobResponseError));

    EXPECT_THROW(pollReadReady(session, &blobMock), ToolException);
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
