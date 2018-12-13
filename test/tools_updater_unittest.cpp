#include "blob_interface_mock.hpp"
#include "data_interface_mock.hpp"
#include "updater.hpp"

#include <string>

#include <gtest/gtest.h>

using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

TEST(UpdaterTest, NormalWalkthroughAllHappy)
{
    /* Call updaterMain and have everything respond happily. */
    DataInterfaceMock handlerMock;
    BlobInterfaceMock blobMock;
    std::string firmwareImage = "image.bin";
    std::string signatureFile = "image.sig";

    std::vector<std::string> blobList = {"/flash/image"};
    StatResponse statObj;
    statObj.blob_state = blobs::FirmwareBlobHandler::UpdateFlags::ipmi |
                         blobs::FirmwareBlobHandler::UpdateFlags::lpc;
    statObj.size = 0;
    blobs::FirmwareBlobHandler::UpdateFlags supported =
        blobs::FirmwareBlobHandler::UpdateFlags::lpc;
    std::uint16_t session = 0xbeef;

    EXPECT_CALL(blobMock, getBlobList()).WillOnce(Return(blobList));

    EXPECT_CALL(blobMock, getStat(StrEq(blobList[0].c_str())))
        .WillOnce(Return(statObj));

    EXPECT_CALL(handlerMock, supportedType()).WillOnce(Return(supported));

    EXPECT_CALL(blobMock, openBlob(StrEq(blobList[0].c_str()), Eq(supported)))
        .WillOnce(Return(session));

    EXPECT_CALL(handlerMock,
                sendContents(StrEq(firmwareImage.c_str()), Eq(session)))
        .WillOnce(Return(true));

    updaterMain(&blobMock, &handlerMock, firmwareImage, signatureFile);
}
