#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace blobs
{
TEST(FirmwareHandlerStatTest, StatOnInactiveBlobIDReturnsTransport)
{
    /* Test that the metadata information returned matches expectations for this
     * case.
     *
     * canHandle has already been called at this point, so we don't need to test
     * the input for this function.
     */

    ImageHandlerMock imageMock;

    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);
    struct BlobMeta meta;
    EXPECT_TRUE(handler->stat("asdf", &meta));
    EXPECT_EQ(FirmwareBlobHandler::UpdateFlags::ipmi, meta.blobState);
}

} // namespace blobs
