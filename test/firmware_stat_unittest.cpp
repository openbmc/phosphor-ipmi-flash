#include "bmc_update_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
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
        {hashBlobId, &imageMock},
        {"asdf", &imageMock},
    };
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock(), CreateUpdateMock());

    blobs::BlobMeta meta;
    EXPECT_TRUE(handler->stat("asdf", &meta));
    EXPECT_EQ(FirmwareBlobHandler::UpdateFlags::ipmi, meta.blobState);
}

} // namespace ipmi_flash
