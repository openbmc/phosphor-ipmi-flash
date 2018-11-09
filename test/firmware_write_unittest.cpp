#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

namespace blobs
{
using ::testing::Eq;
using ::testing::Return;

TEST(FirmwareHandlerWriteTest, DataTypeIpmiWriteSuccess)
{
    /* Verify if data type ipmi, it calls write with the bytes. */

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

    std::vector<std::uint8_t> bytes = {0xaa, 0x55};

    EXPECT_CALL(imageMock2, write(0, Eq(bytes))).WillOnce(Return(true));
    EXPECT_TRUE(handler->write(0, 0, bytes));
}

TEST(FirmwareHandlerWriteTest, DataTypeNonIpmiWriteSuccess)
{
    /* Verify if data type non-ipmi, it calls write with the length. */

    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    DataHandlerMock dataMock;

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc, "asdf"));

    struct ExtChunkHdr request;
    request.length = 4; /* number of bytes to read. */
    std::vector<std::uint8_t> ipmiRequest;
    ipmiRequest.resize(sizeof(request));
    std::memcpy(ipmiRequest.data(), &request, sizeof(request));

    std::vector<std::uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};

    EXPECT_CALL(dataMock, copyFrom(request.length)).WillOnce(Return(bytes));
    EXPECT_CALL(imageMock2, write(0, Eq(bytes))).WillOnce(Return(true));
    EXPECT_TRUE(handler->write(0, 0, ipmiRequest));
}

TEST(FirmwareHandlerWriteTest, DataTypeNonIpmiWriteFailsBadRequest)
{
    /* Verify the data type non-ipmi, if the request's structure doesn't match,
     * return failure. */

    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    DataHandlerMock dataMock;

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(blobs, data);

    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc, "asdf"));

    struct ExtChunkHdr request;
    request.length = 4; /* number of bytes to read. */

    std::vector<std::uint8_t> ipmiRequest;
    ipmiRequest.resize(sizeof(request));
    std::memcpy(ipmiRequest.data(), &request, sizeof(request));
    ipmiRequest.push_back(1);

    /* ipmiRequest is too large by one byte. */
    EXPECT_FALSE(handler->write(0, 0, ipmiRequest));
}

} // namespace blobs
