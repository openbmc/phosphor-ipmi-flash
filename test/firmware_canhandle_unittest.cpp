#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"

#include <sdbusplus/test/sdbus_mock.hpp>
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
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
        {"bcdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data, "");

    for (const auto& item : items)
    {
        EXPECT_EQ(item.expected, handler->canHandleBlob(item.name));
    }
}
} // namespace blobs
