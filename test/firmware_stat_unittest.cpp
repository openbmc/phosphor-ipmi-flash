#include "firmware_handler.hpp"

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

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        {"asdf"}, static_cast<uint16_t>(FirmwareUpdateFlags::bt));
    struct BlobMeta meta;
    EXPECT_TRUE(handler->stat("asdf", &meta));
    EXPECT_EQ(static_cast<uint16_t>(FirmwareUpdateFlags::bt), meta.blobState);
}

} // namespace blobs
