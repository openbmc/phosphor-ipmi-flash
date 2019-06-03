#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"

#include <memory>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

class FirmwareHandlerOpenTestIpmiOnly : public IpmiOnlyFirmwareTest
{
};

class FirmwareHandlerOpenTestLpc : public FakeLpcFirmwareTest
{
};

TEST_F(FirmwareHandlerOpenTestIpmiOnly, OpenWithEverythingValid)
{
    /* The client passes write set, and a transport that's supported, and a
     * firmware image blob_id that's supported. */
    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        "asdf"));

    /* The active image blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeImageBlobId));
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly, OpenWithEverythingValidHashFile)
{
    /* Open the hash file by blob_id. */
    EXPECT_CALL(imageMock, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        hashBlobId));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));
}

TEST_F(FirmwareHandlerOpenTestLpc, OpenWithDataHandlerAllSucceeds)
{
    /* Attempting to open a file that has an active handler, and use that active
     * handler method.
     */
    EXPECT_CALL(dataMock, open()).WillOnce(Return(true));
    EXPECT_CALL(imageMock, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc,
        hashBlobId));

    /* The active hash blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeHashBlobId));
}

TEST_F(FirmwareHandlerOpenTestLpc, OpenWithDataHandlerReturnsFailure)
{
    /* The data handler call returns failure on open, therefore open fails. */
    EXPECT_CALL(dataMock, open()).WillOnce(Return(false));

    EXPECT_FALSE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::lpc,
        hashBlobId));

    /* The active hash blob_id was not added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(2, currentBlobs.size());
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly,
       OpenEverythingSucceedsVerifyOpenFileCheck)
{
    /* Verify only one file can be open at a time by opening a file, trying
     * again, then closing, and trying again.
     */
    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        "asdf"));

    /* The active image blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeImageBlobId));

    /* Open the hash file (since we opened an image file). */
    EXPECT_FALSE(handler->open(
        1, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        hashBlobId));

    EXPECT_TRUE(handler->close(0));

    EXPECT_CALL(imageMock, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        1, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        hashBlobId));
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly, OpenEverythingSucceedsOpenActiveFails)
{
    /* Attempting to open the active image blob, when it's present will fail.
     *
     * TODO: We'll need another test that closes first because you can only have
     * one file open at a time.
     */
    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        "asdf"));

    /* The active image blob_id was added. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(3, currentBlobs.size());
    EXPECT_EQ(1, std::count(currentBlobs.begin(), currentBlobs.end(),
                            activeImageBlobId));

    /* Close only active session, to verify it's failing on attempt to open a
     * specific blob_id.
     */
    EXPECT_TRUE(handler->close(0));

    EXPECT_FALSE(handler->open(
        1, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        activeImageBlobId));
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly,
       OpenWithEverythingValidImageHandlerFails)
{
    /* The image handler for a specific type of image is allowed to return
     * failure on open.  let's simulate that. */
    EXPECT_CALL(imageMock, open("asdf")).WillOnce(Return(false));

    EXPECT_FALSE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        "asdf"));

    /* Verify blob_id list doesn't grow. */
    auto currentBlobs = handler->getBlobIds();
    EXPECT_EQ(2, currentBlobs.size());
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly, OpenWithoutWriteFails)
{
    /* The client must set the file write bit. */
    EXPECT_FALSE(
        handler->open(0, FirmwareBlobHandler::UpdateFlags::ipmi, "asdf"));
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly, OpenWithInvalidTransportBit)
{
    /* The client sends a request with a transport mechanism not supported. */
    EXPECT_FALSE(
        handler->open(0, FirmwareBlobHandler::UpdateFlags::lpc, "asdf"));
}

TEST_F(FirmwareHandlerOpenTestIpmiOnly, OpenWithInvalidImageBlobId)
{
    /* The client sends a request with an invalid image blob_id. */
    EXPECT_FALSE(handler->open(
        0, blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi,
        "bcdf"));
}

/* TODO: The client sends a request during verification. */
/* TODO: The client sends a second request to open when there is already an open
 * file.
 */
/* TODO: The client sends a request to open active image. */
/* TODO: The client sends a request to open active hash. */

} // namespace
} // namespace ipmi_flash
