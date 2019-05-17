#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace blobs
{
using ::testing::_;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::StrictMock;

TEST(FirmwareHandlerCommitTest, VerifyCannotCommitOnFlashImage)
{
    /* Verify the flash image returns failure on this command.  It's a fairly
     * artificial test.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

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
        {hashBlobId, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    EXPECT_CALL(imageMock1, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        hashBlobId));

    EXPECT_FALSE(handler->commit(0, {}));
}

TEST(FirmwareHandlerCommitTest, VerifyCommitAcceptedOnVerifyBlob)
{
    /* Verify the verify blob lets you call this command, and it returns
     * success.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    EXPECT_TRUE(handler->open(0, OpenFlags::write, verifyBlobId));

    EXPECT_TRUE(handler->commit(0, {}));
}

TEST(FirmwareHandlerCommitTest, VerifyCommitCanOnlyBeCalledOnceForEffect)
{
    /* Verify you cannot call the commit() command once verification is
     * started, after which it will just return true.
     */
    ImageHandlerMock imageMock1, imageMock2;
    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock1},
        {"asdf", &imageMock2},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
    };

    auto verifyMock = CreateVerifyMock();
    auto verifyMockPtr = reinterpret_cast<VerificationMock*>(verifyMock.get());

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, std::move(verifyMock));

    EXPECT_TRUE(handler->open(0, OpenFlags::write, verifyBlobId));

    EXPECT_CALL(*verifyMockPtr, triggerVerification())
        .WillRepeatedly(Return(true));

    EXPECT_TRUE(handler->commit(0, {}));
    EXPECT_TRUE(handler->commit(0, {}));
}

} // namespace blobs
