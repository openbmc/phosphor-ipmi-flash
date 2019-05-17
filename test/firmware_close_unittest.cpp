#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

TEST(FirmwareHandlerCloseTest, CloseSuceedsWithDataHandler)
{
    /* Boring test where you open a blob_id, then verify that when it's closed
     * everything looks right.
     */
    DataHandlerMock dataMock;
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    EXPECT_CALL(dataMock, open()).WillOnce(Return(true));
    EXPECT_CALL(imageMock, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc,
        hashBlobId));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(4, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));

    /* Set up close() expectations. */
    EXPECT_CALL(dataMock, close());
    EXPECT_CALL(imageMock, close());
    EXPECT_TRUE(handler->close(0));

    /* Close does not delete the active blob id.  This indicates that there is
     * data queued.
     */
}

TEST(FirmwareHandlerCloseTest, CloseSuceedsWithoutDataHandler)
{
    /* Boring test where you open a blob_id using ipmi, so there's no data
     * handler, and it's closed and everything looks right.
     */
    DataHandlerMock dataMock;
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    EXPECT_CALL(imageMock, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        hashBlobId));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(4, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));

    /* Set up close() expectations. */
    EXPECT_CALL(imageMock, close());
    EXPECT_TRUE(handler->close(0));
}

} // namespace blobs
