#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Eq;
using ::testing::Return;

TEST(FirmwareSessionStateTest, DataTypeIpmiNoMetadata)
{
    /* Verifying running stat if the type of data session is IPMI returns no
     * metadata.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    int size = 512;
    EXPECT_CALL(imageMock2, getSize()).WillOnce(Return(size));

    struct BlobMeta meta;
    EXPECT_TRUE(handler->stat(0, &meta));
    EXPECT_EQ(meta.blobState,
              OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi);
    EXPECT_EQ(meta.size, size);
    EXPECT_EQ(meta.metadata.size(), 0);
}

TEST(FirmwareSessionStateTest, DataTypeP2AReturnsMetadata)
{
    /* Really any type that isn't IPMI can return metadata, but we only expect
     * P2A to for now.  Later, LPC may have reason to provide data, and can by
     * simply implementing read().
     */
}

} // namespace blobs
