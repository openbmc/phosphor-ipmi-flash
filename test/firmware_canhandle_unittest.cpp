#include "firmware_handler.hpp"

#include <memory>
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

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        {"asdf", "bcdf"}, static_cast<uint16_t>(FirmwareUpdateFlags::bt));

    for (const auto& item : items)
    {
        EXPECT_EQ(item.expected, handler->canHandleBlob(item.name));
    }
}
} // namespace blobs
