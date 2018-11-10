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

TEST(FirmwareHandlerOpenTest, OpenWithEverythingValid)
{
    /* The client passes write set, and a transport that's supported, and a
     * firmware image blob_id that's supported. */

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    /* The active image blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeImageBlobID));
}

TEST(FirmwareHandlerOpenTest, OpenWithEverythingValidHashFile)
{
    /* Open the hash file by blob_id. */
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock, open(Eq(FirmwareBlobHandler::hashBlobID)))
        .WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        FirmwareBlobHandler::hashBlobID));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeHashBlobID));
}

TEST(FirmwareHandlerOpenTest, OpenWithDataHandlerAllSucceeds)
{
    /* Attempting to open a file that has an active handler, and use that active
     * handler method.
     */
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

    EXPECT_CALL(dataMock, open()).WillOnce(Return(true));
    EXPECT_CALL(imageMock, open(Eq(FirmwareBlobHandler::hashBlobID)))
        .WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc,
        FirmwareBlobHandler::hashBlobID));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeHashBlobID));
}

TEST(FirmwareHandlerOpenTest, OpenWithDataHandlerReturnsFailure)
{
    /* The data handler call returns failure on open, therefore open fails. */
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

    EXPECT_CALL(dataMock, open()).WillOnce(Return(false));

    EXPECT_FALSE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc,
        FirmwareBlobHandler::hashBlobID));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(2, currentBlobs.size());
}

TEST(FirmwareHandlerOpenTest, OpenEverythingSucceedsVerifyOpenFileCheck)
{
    /* Verify only one file can be open at a time by opening a file, trying
     * again, then closing, and trying again.
     */
    ImageHandlerMock imageMock1, imageMock2;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    /* The active image blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeImageBlobID));

    /* Open the hash file (since we opened an image file). */
    EXPECT_FALSE(handler->open(
        1, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        FirmwareBlobHandler::hashBlobID));

    /* Close the file, currently ignoring its return value. */
    handler->close(0);

    EXPECT_CALL(imageMock1, open(StrEq(FirmwareBlobHandler::hashBlobID)))
        .WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        1, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        FirmwareBlobHandler::hashBlobID));
}

TEST(FirmwareHandlerOpenTest, OpenEverythingSucceedsOpenActiveFails)
{
    /* Attempting to open the active image blob, when it's present will fail.
     *
     * TODO: We'll need another test that closes first because you can only have
     * one file open at a time.
     */
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    /* The active image blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            FirmwareBlobHandler::activeImageBlobID));

    /* Close only active session, to verify it's failing on attempt to open a
     * specific blob_id.
     */
    handler->close(0);

    EXPECT_FALSE(handler->open(
        1, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        FirmwareBlobHandler::activeImageBlobID));
}

TEST(FirmwareHandlerOpenTest, OpenWithEverythingValidImageHandlerFails)
{
    /* The image handler for a specific type of image is allowed to return
     * failure on open.  let's simulate that. */

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(false));

    EXPECT_FALSE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    /* Verify blob_id list doesn't grow. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(2, currentBlobs.size());
}

TEST(FirmwareHandlerOpenTest, OpenWithoutWriteFails)
{
    /* The client must set the file write bit. */

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_FALSE(
        handler->open(0, FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));
}

TEST(FirmwareHandlerOpenTest, OpenWithInvalidTransportBit)
{
    /* The client sends a request with a transport mechanism not supported. */

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_FALSE(
        handler->open(0, FirmwareBlobHandler::UpdateFlags::lpc, "asdf"));
}

TEST(FirmwareHandlerOpenTest, OpenWithInvalidImageBlobId)
{
    /* The client sends a request with an invalid image blob_id. */

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_FALSE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "bcdf"));
}

/* TODO: The client sends a request during verification. */
/* TODO: The client sends a second request to open when there is already an open
 * file.
 */
/* TODO: The client sends a request to open active image. */
/* TODO: The client sends a request to open active hash. */

} // namespace blobs
