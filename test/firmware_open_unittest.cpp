#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Return;

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
