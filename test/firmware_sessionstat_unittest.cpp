#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Eq;
using ::testing::Return;

class FirmwareSessionStateTest : public ::testing::Test
{
  protected:
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs;

    void SetUp() override
    {
        blobs = {
            {hashBlobId, &imageMock1},
            {"asdf", &imageMock2},
        };
    }
};

TEST_F(FirmwareSessionStateTest, DataTypeIpmiNoMetadata)
{
    /* Verifying running stat if the type of data session is IPMI returns no
     * metadata.
     */
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

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

TEST_F(FirmwareSessionStateTest, DataTypeP2AReturnsMetadata)
{
    /* Really any type that isn't IPMI can return metadata, but we only expect
     * P2A to for now.  Later, LPC may have reason to provide data, and can by
     * simply implementing read().
     */
    DataHandlerMock dataMock;

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    EXPECT_CALL(dataMock, open()).WillOnce(Return(true));
    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc, "asdf"));

    int size = 512;
    EXPECT_CALL(imageMock2, getSize()).WillOnce(Return(size));
    std::vector<std::uint8_t> mBytes = {0x01, 0x02};
    EXPECT_CALL(dataMock, readMeta()).WillOnce(Return(mBytes));

    struct BlobMeta meta;
    EXPECT_TRUE(handler->stat(0, &meta));
    EXPECT_EQ(meta.blobState,
              OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc);
    EXPECT_EQ(meta.size, size);
    EXPECT_EQ(meta.metadata.size(), mBytes.size());
    EXPECT_EQ(meta.metadata[0], mBytes[0]);
    EXPECT_EQ(meta.metadata[1], mBytes[1]);
}

} // namespace blobs
