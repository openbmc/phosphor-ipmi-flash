#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace blobs
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
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
        {"bcdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::FirmwareUpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    for (const auto& item : items)
    {
        EXPECT_EQ(item.expected, handler->canHandleBlob(item.name));
    }
}
} // namespace blobs
