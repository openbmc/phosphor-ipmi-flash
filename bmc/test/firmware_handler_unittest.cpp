#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::UnorderedElementsAreArray;

TEST(FirmwareHandlerTest, CreateEmptyListVerifyFails)
{
    std::vector<DataHandlerPack> data = {
        {FirmwareFlags::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        {}, data, std::move(CreateActionMap("abcd")));
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
        blobs, {}, std::move(CreateActionMap("asdf")));
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
        {FirmwareFlags::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, std::move(CreateActionMap("asdf")));
    EXPECT_EQ(handler, nullptr);

    blobs.push_back({hashBlobId, &imageMock});

    handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, std::move(CreateActionMap("asdf")));

    auto result = handler->getBlobIds();
    std::vector<std::string> expectedBlobs = {"asdf", hashBlobId};
    EXPECT_THAT(result, UnorderedElementsAreArray(expectedBlobs));
}

} // namespace
} // namespace ipmi_flash
