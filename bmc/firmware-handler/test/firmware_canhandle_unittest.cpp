#include "create_action_map.hpp"
#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_mock.hpp"
#include "util.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
TEST(FirmwareHandlerCanHandleTest, VerifyItemsInListAreOk)
{
    struct ListItem
    {
        std::string name;
        bool expected;
    };

    std::vector<ListItem> items = {
        {"asdf", true}, {"nope", false}, {"123123", false}, {"bcdf", true}};

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs;
    blobs.emplace_back(hashBlobId, std::make_unique<ImageHandlerMock>());
    blobs.emplace_back("asdf", std::make_unique<ImageHandlerMock>());
    blobs.emplace_back("bcdf", std::make_unique<ImageHandlerMock>());

    std::vector<DataHandlerPack> data;
    data.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::move(data), CreateActionMap("asdf"));

    for (const auto& item : items)
    {
        EXPECT_EQ(item.expected, handler->canHandleBlob(item.name));
    }
}
} // namespace ipmi_flash
