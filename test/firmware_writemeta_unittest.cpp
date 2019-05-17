#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Eq;
using ::testing::Return;

TEST(FirmwareHandlerWriteMetaTest, WriteConfigParametersFailIfOverIPMI)
{
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock1},
        {"asdf", &imageMock2},
    };

    DataHandlerMock dataMock;

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    std::vector<std::uint8_t> bytes = {0xaa, 0x55};

    EXPECT_FALSE(handler->writeMeta(0, 0, bytes));
}

TEST(FirmwareHandlerWriteMetaTest, WriteConfigParametersPassedThrough)
{
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock1},
        {"asdf", &imageMock2},
    };

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

    std::vector<std::uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};

    EXPECT_CALL(dataMock, writeMeta(Eq(bytes))).WillOnce(Return(true));
    EXPECT_TRUE(handler->writeMeta(0, 0, bytes));
}

} // namespace blobs
