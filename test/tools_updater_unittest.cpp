#include "data_interface_mock.hpp"
#include "updater.hpp"

#include <blobs-ipmid/blobs.hpp>
#include <ipmiblob/test/blob_interface_mock.hpp>
#include <string>

#include <gtest/gtest.h>

namespace host_tool
{

using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

TEST(UpdaterTest, NormalWalkthroughAllHappy)
{
    /* Call updaterMain and have everything respond happily. */
    DataInterfaceMock handlerMock;
    ipmiblob::BlobInterfaceMock blobMock;
    std::string firmwareImage = "image.bin";
    std::string signatureFile = "image.sig";
    std::string expectedBlob = "/flash/image";

    std::vector<std::string> blobList = {expectedBlob};
    ipmiblob::StatResponse statObj;
    statObj.blob_state = blobs::FirmwareBlobHandler::UpdateFlags::ipmi |
                         blobs::FirmwareBlobHandler::UpdateFlags::lpc;
    statObj.size = 0;
    std::uint16_t supported =
        static_cast<std::uint16_t>(
            blobs::FirmwareBlobHandler::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(blobs::OpenFlags::write);
    std::uint16_t session = 0xbeef;

    EXPECT_CALL(blobMock, getBlobList()).WillOnce(Return(blobList));

    EXPECT_CALL(blobMock, getStat(Eq(expectedBlob))).WillOnce(Return(statObj));

    EXPECT_CALL(handlerMock, supportedType())
        .WillOnce(Return(blobs::FirmwareBlobHandler::UpdateFlags::lpc));

    EXPECT_CALL(blobMock, openBlob(StrEq(expectedBlob.c_str()), Eq(supported)))
        .WillOnce(Return(session));

    EXPECT_CALL(handlerMock,
                sendContents(StrEq(firmwareImage.c_str()), Eq(session)))
        .WillOnce(Return(true));

    updaterMain(&blobMock, &handlerMock, firmwareImage, signatureFile);
}

} // namespace host_tool
