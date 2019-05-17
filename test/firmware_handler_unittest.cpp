#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"

#include <algorithm>
#include <sdbusplus/test/sdbus_mock.hpp>
#include <vector>

#include <gtest/gtest.h>

namespace blobs
{

TEST(FirmwareHandlerTest, CreateEmptyListVerifyFails)
{
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), {}, data);
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, CreateEmptyDataHandlerListFails)
{
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, {});
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

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data);
    EXPECT_EQ(handler, nullptr);

    blobs.push_back({hashBlobId, &imageMock});

    handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data);
    auto result = handler->getBlobIds();
    EXPECT_EQ(3, result.size());
    EXPECT_EQ(3, std::count(result.begin(), result.end(), "asdf") +
                     std::count(result.begin(), result.end(), hashBlobId) +
                     std::count(result.begin(), result.end(), verifyBlobId));
}
} // namespace blobs
