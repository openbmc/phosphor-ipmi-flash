#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace blobs
{

TEST(FirmwareHandlerTest, CreateEmptyListVerifyFails)
{
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::FirmwareUpdateFlags::bt, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler({}, data);
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, CreateEmptyDataHandlerListFails)
{
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {"asdf", &imageMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, {});
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, CreateEmptyListVerifyHasHash)
{
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::FirmwareUpdateFlags::bt, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);
    auto result = handler->getBlobIds();
    EXPECT_EQ(2, result.size());
    EXPECT_EQ(2, std::count(result.begin(), result.end(), "asdf") +
                     std::count(result.begin(), result.end(),
                                FirmwareBlobHandler::hashBlobID));
}
} // namespace blobs
