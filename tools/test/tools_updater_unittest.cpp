#include "data_interface_mock.hpp"
#include "updater.hpp"
#include "updater_mock.hpp"
#include "util.hpp"

#include <blobs-ipmid/blobs.hpp>
#include <ipmiblob/test/blob_interface_mock.hpp>
#include <string>

#include <gtest/gtest.h>

namespace host_tool
{

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::TypedEq;

TEST(UpdaterTest, CheckAvailableSuccess)
{
    /* Call checkAvailable directly() to make sure it works. */
    DataInterfaceMock handlerMock;
    ipmiblob::BlobInterfaceMock blobMock;

    ipmiblob::StatResponse statObj;
    statObj.blob_state = ipmi_flash::FirmwareBlobHandler::UpdateFlags::ipmi |
                         ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc;
    statObj.size = 0;

    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(
            Return(std::vector<std::string>({ipmi_flash::staticLayoutBlobId})));
    EXPECT_CALL(blobMock, getStat(TypedEq<const std::string&>(
                              ipmi_flash::staticLayoutBlobId)))
        .WillOnce(Return(statObj));

    EXPECT_CALL(handlerMock, supportedType())
        .WillOnce(Return(ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc));

    UpdateHandler updater(&blobMock, &handlerMock);
    EXPECT_TRUE(updater.checkAvailable(ipmi_flash::staticLayoutBlobId));
}

TEST(UpdaterTest, SendFileSuccess)
{
    /* Call sendFile to verify it does what we expect. */
    DataInterfaceMock handlerMock;
    ipmiblob::BlobInterfaceMock blobMock;

    std::string firmwareImage = "image.bin";

    std::uint16_t supported =
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(blobs::OpenFlags::write);
    std::uint16_t session = 0xbeef;

    EXPECT_CALL(handlerMock, supportedType())
        .WillOnce(Return(ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc));

    EXPECT_CALL(
        blobMock,
        openBlob(StrEq(ipmi_flash::staticLayoutBlobId.c_str()), supported))
        .WillOnce(Return(session));

    EXPECT_CALL(handlerMock,
                sendContents(StrEq(firmwareImage.c_str()), session))
        .WillOnce(Return(true));

    EXPECT_CALL(blobMock, closeBlob(session)).Times(1);

    UpdateHandler updater(&blobMock, &handlerMock);
    updater.sendFile(ipmi_flash::staticLayoutBlobId, firmwareImage);
}

#if 0 /* TODO: fix this up. */
TEST(UpdaterTest, NormalWalkthroughAllHappy)
{
    /* Call updaterMain and have everything respond happily. */
    DataInterfaceMock handlerMock;
    ipmiblob::BlobInterfaceMock blobMock;

    UpdateHandlerMock updaterMock;

    std::string firmwareImage = "image.bin";
    std::string signatureFile = "image.sig";

    std::vector<std::string> blobList = {ipmi_flash::staticLayoutBlobId};
    ipmiblob::StatResponse statObj;
    statObj.blob_state = ipmi_flash::FirmwareBlobHandler::UpdateFlags::ipmi |
                         ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc;
    statObj.size = 0;
    std::uint16_t supported =
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(blobs::OpenFlags::write);
    std::uint16_t session = 0xbeef;

    EXPECT_CALL(blobMock, getBlobList()).WillOnce(Return(blobList));

    EXPECT_CALL(blobMock, getStat(TypedEq<const std::string&>(ipmi_flash::staticLayoutBlobId)))
        .WillOnce(Return(statObj));

    EXPECT_CALL(handlerMock, supportedType())
        .WillOnce(Return(ipmi_flash::FirmwareBlobHandler::UpdateFlags::lpc));

    EXPECT_CALL(blobMock, openBlob(StrEq(ipmi_flash::staticLayoutBlobId.c_str()), Eq(supported)))
        .WillOnce(Return(session));

    EXPECT_CALL(handlerMock,
                sendContents(StrEq(firmwareImage.c_str()), Eq(session)))
        .WillOnce(Return(true));

    EXPECT_CALL(blobMock, openBlob(StrEq(blobs::hashBlobId.c_str()), Eq(supported)))
        .WillOnce(Return(session));

    EXPECT_CALL(handlerMock,
                sendContents(StrEq(signatureFile.c_str()), Eq(session)))
        .WillOnce(Return(true));

    EXPECT_CALL(blobMock,
                openBlob(StrEq(blobs::verifyBlobId.c_str()), Eq(supported)))
        .WillOnce(Return(session));

    EXPECT_CALL(blobMock, commit(session, _)).WillOnce(Return());

    ipmiblob::StatResponse verificationResponse;
    verificationResponse.blob_state = supported | blobs::StateFlags::committing;
    verificationResponse.size = 0;
    verificationResponse.metadata.push_back(static_cast<std::uint8_t>(
        ipmi_flash::FirmwareBlobHandler::ActionStatus::success));

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(verificationResponse));

    updaterMain(&blobMock, &handlerMock, firmwareImage, signatureFile);
}
#endif

} // namespace host_tool
