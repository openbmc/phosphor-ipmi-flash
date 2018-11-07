#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <memory>

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
        {"asdf", &imageMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, static_cast<uint16_t>(FirmwareUpdateFlags::bt));
    struct BlobMeta meta;
    EXPECT_TRUE(handler->stat("asdf", &meta));
    EXPECT_EQ(static_cast<uint16_t>(FirmwareUpdateFlags::bt), meta.blobState);
}

} // namespace blobs
