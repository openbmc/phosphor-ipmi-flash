#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"

#include <sdbusplus/test/sdbus_mock.hpp>
#include <vector>

#include <gtest/gtest.h>

namespace blobs
{
using ::testing::_;
using ::testing::Return;

TEST(FirmwareHandlerCommitTest, VerifyCannotCommitOnFlashImage)
{
    /* Verify the flash image returns failure on this command.  It's a fairly
     * artificial test.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data);

    EXPECT_CALL(imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));

    EXPECT_FALSE(handler->commit(0, {}));
}

TEST(FirmwareHandlerCommitTest, VerifyCannotCommitOnHashFile)
{
    /* Verify the hash file returns failure on this command.  It's a fairly
     * artificial test.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data);

    EXPECT_CALL(imageMock1, open(FirmwareBlobHandler::hashBlobID))
        .WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        FirmwareBlobHandler::hashBlobID));

    EXPECT_FALSE(handler->commit(0, {}));
}

TEST(FirmwareHandlerCommitTest, VerifyCommitAcceptedOnVerifyBlob)
{
    /* Verify the verify blob lets you call this command, and it returns
     * success.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data);

    EXPECT_TRUE(
        handler->open(0, OpenFlags::write, FirmwareBlobHandler::verifyBlobID));

    EXPECT_TRUE(handler->commit(0, {}));
}

TEST(FirmwareHandlerCommitTest, VerifyCommitCanOnlyBeCalledOnce)
{
    /* Verify you cannot call the commit() command once verification is started.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {FirmwareBlobHandler::hashBlobID, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    sdbusplus::SdBusMock sdbus_mock;
    auto bus_mock = sdbusplus::get_mocked_new(&sdbus_mock);

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(bus_mock), blobs, data);

    EXPECT_TRUE(
        handler->open(0, OpenFlags::write, FirmwareBlobHandler::verifyBlobID));

    EXPECT_CALL(sdbus_mock, sd_bus_call(_, _, _, _, _)).WillOnce(Return(0));

    EXPECT_TRUE(handler->commit(0, {}));
    EXPECT_FALSE(handler->commit(0, {}));
}

} // namespace blobs
