#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
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

    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
        {"bcdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareFlags::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, std::move(CreateActionMap("asdf")));

    for (const auto& item : items)
    {
        EXPECT_EQ(item.expected, handler->canHandleBlob(item.name));
    }
}
} // namespace ipmi_flash
