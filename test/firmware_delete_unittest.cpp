#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

TEST(FirmwareHandlerDeleteTest, DeleteActiveHashSucceeds)
{
    /* Delete active image succeeds. */
    DataHandlerMock dataMock;
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock, open(Eq(FirmwareBlobHandler::hashBlobID)))
        .WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        FirmwareBlobHandler::hashBlobID));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(4, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeHashBlobID));

    /* Set up close() expectations. */
    EXPECT_CALL(imageMock, close());
    EXPECT_TRUE(handler->close(0));

    currentBlobs = handler->getBlobIds();
    EXPECT_EQ(4, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeHashBlobID));

    /* Delete the blob. */
    EXPECT_TRUE(handler->deleteBlob(FirmwareBlobHandler::activeHashBlobID));

    currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(0, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeHashBlobID));
}

} // namespace blobs
