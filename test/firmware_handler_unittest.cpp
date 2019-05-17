#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

namespace blobs
{

TEST(FirmwareHandlerTest, CreateEmptyListVerifyFails)
{
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        {}, data, CreateVerifyMock());
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, CreateEmptyDataHandlerListFails)
{
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, {}, CreateVerifyMock());
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, VerifyHashRequiredForHappiness)
{
    /* This works fine only if you also pass in the hash handler. */
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());
    EXPECT_EQ(handler, nullptr);

    blobs.push_back({hashBlobId, &imageMock});

    handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());
    auto result = handler->getBlobIds();
    EXPECT_EQ(3, result.size());
    EXPECT_EQ(3, std::count(result.begin(), result.end(), "asdf") +
                     std::count(result.begin(), result.end(), hashBlobId) +
                     std::count(result.begin(), result.end(), verifyBlobId));
}
} // namespace blobs
