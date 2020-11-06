#include "create_action_map.hpp"
#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_mock.hpp"
#include "util.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::UnorderedElementsAreArray;

TEST(FirmwareHandlerTest, CreateEmptyHandlerListVerifyFails)
{
    std::vector<DataHandlerPack> data;
    data.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        {}, std::move(data), CreateActionMap("abcd"));
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, CreateEmptyDataHandlerListFails)
{
    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs;
    blobs.emplace_back(hashBlobId, std::make_unique<ImageHandlerMock>());
    blobs.emplace_back("asdf", std::make_unique<ImageHandlerMock>());

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::vector<DataHandlerPack>(),
        CreateActionMap("asdf"));
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, CreateEmptyActionPackVerifyFails)
{
    /* The ActionPack map corresponds to the firmware list passed in, but
     * they're not checked against each other yet.
     */
    std::vector<DataHandlerPack> data;
    data.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    std::vector<HandlerPack> blobs;
    blobs.emplace_back("asdf", std::make_unique<ImageHandlerMock>());
    blobs.emplace_back(hashBlobId, std::make_unique<ImageHandlerMock>());

    ActionMap emptyMap;

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::move(data), std::move(emptyMap));
    EXPECT_EQ(handler, nullptr);
}
TEST(FirmwareHandlerTest, FirmwareHandlerListRequiresAtLeastTwoEntries)
{
    /* The hashblob handler must be one of the entries, but it cannot be the
     * only entry.
     */
    std::vector<DataHandlerPack> data;
    data.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    /* Provide a firmware list that has the hash blob, which is the required one
     * -- tested in a different test.
     */
    std::vector<HandlerPack> blobs;
    blobs.emplace_back(hashBlobId, std::make_unique<ImageHandlerMock>());

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::move(data), CreateActionMap("asdf"));
    EXPECT_EQ(handler, nullptr);

    /* Add second firmware and it'll now work. */
    std::vector<HandlerPack> blobs2;
    blobs2.emplace_back(hashBlobId, std::make_unique<ImageHandlerMock>());
    blobs2.emplace_back("asdf", std::make_unique<ImageHandlerMock>());

    std::vector<DataHandlerPack> data2;
    data2.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs2), std::move(data2), CreateActionMap("asdf"));

    auto result = handler->getBlobIds();
    std::vector<std::string> expectedBlobs = {"asdf", hashBlobId};
    EXPECT_THAT(result, UnorderedElementsAreArray(expectedBlobs));
}
TEST(FirmwareHandlerTest, VerifyHashRequiredForHappiness)
{
    std::vector<DataHandlerPack> data;
    data.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    /* This works fine only if you also pass in the hash handler. */
    std::vector<HandlerPack> blobs;
    blobs.emplace_back("asdf", std::make_unique<ImageHandlerMock>());

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::move(data), CreateActionMap("asdf"));
    EXPECT_EQ(handler, nullptr);

    std::vector<HandlerPack> blobs2;
    blobs2.emplace_back("asdf", std::make_unique<ImageHandlerMock>());
    blobs2.emplace_back(hashBlobId, std::make_unique<ImageHandlerMock>());

    std::vector<DataHandlerPack> data2;
    data2.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs2), std::move(data2), CreateActionMap("asdf"));

    auto result = handler->getBlobIds();
    std::vector<std::string> expectedBlobs = {"asdf", hashBlobId};
    EXPECT_THAT(result, UnorderedElementsAreArray(expectedBlobs));
}

} // namespace
} // namespace ipmi_flash
