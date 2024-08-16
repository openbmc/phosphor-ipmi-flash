#include "data_interface_mock.hpp"
#include "flags.hpp"
#include "status.hpp"
#include "tool_errors.hpp"
#include "updater.hpp"
#include "updater_mock.hpp"
#include "util.hpp"

#include <blobs-ipmid/blobs.hpp>
#include <ipmiblob/blob_errors.hpp>
#include <ipmiblob/test/blob_interface_mock.hpp>

#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace host_tool
{

using ::testing::_;
using ::testing::Return;
using ::testing::Throw;
using ::testing::TypedEq;

class UpdateHandlerTest : public ::testing::Test
{
  protected:
    const std::uint16_t session = 0xbeef;

    DataInterfaceMock handlerMock;
    ipmiblob::BlobInterfaceMock blobMock;
    UpdateHandler updater{&blobMock, &handlerMock};
};

TEST_F(UpdateHandlerTest, CheckAvailableSuccess)
{
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(
            Return(std::vector<std::string>({ipmi_flash::staticLayoutBlobId})));

    EXPECT_TRUE(updater.checkAvailable(ipmi_flash::staticLayoutBlobId));
}

TEST_F(UpdateHandlerTest, CheckAvailableFailure)
{
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(Return(std::vector<std::string>()));

    EXPECT_FALSE(updater.checkAvailable(ipmi_flash::staticLayoutBlobId));
}

TEST_F(UpdateHandlerTest, SendFileSuccess)
{
    /* Call sendFile to verify it does what we expect. */
    std::string firmwareImage = "image.bin";

    std::uint16_t supported =
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::openWrite);

    EXPECT_CALL(handlerMock, supportedType())
        .WillOnce(Return(ipmi_flash::FirmwareFlags::UpdateFlags::lpc));

    EXPECT_CALL(blobMock, openBlob(ipmi_flash::staticLayoutBlobId, supported))
        .WillOnce(Return(session));

    EXPECT_CALL(handlerMock, sendContents(firmwareImage, session))
        .WillOnce(Return(true));

    EXPECT_CALL(blobMock, closeBlob(session)).Times(1);

    updater.sendFile(ipmi_flash::staticLayoutBlobId, firmwareImage);
}

TEST_F(UpdateHandlerTest, SendFileExceptsOnBlobOpening)
{
    std::string firmwareImage = "image.bin";

    std::uint16_t supported =
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::openWrite);

    EXPECT_CALL(handlerMock, supportedType())
        .WillOnce(Return(ipmi_flash::FirmwareFlags::UpdateFlags::lpc));

    EXPECT_CALL(blobMock, openBlob(ipmi_flash::staticLayoutBlobId, supported))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));

    EXPECT_THROW(
        updater.sendFile(ipmi_flash::staticLayoutBlobId, firmwareImage),
        ToolException);
}

TEST_F(UpdateHandlerTest, SendFileHandlerFailureCausesException)
{
    std::string firmwareImage = "image.bin";

    std::uint16_t supported =
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::openWrite);

    EXPECT_CALL(handlerMock, supportedType())
        .WillRepeatedly(Return(ipmi_flash::FirmwareFlags::UpdateFlags::lpc));

    EXPECT_CALL(blobMock, openBlob(ipmi_flash::staticLayoutBlobId, supported))
        .WillRepeatedly(Return(session));

    EXPECT_CALL(handlerMock, sendContents(firmwareImage, session))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(blobMock, closeBlob(session)).Times(3);

    EXPECT_THROW(
        updater.sendFile(ipmi_flash::staticLayoutBlobId, firmwareImage),
        ToolException);
}

TEST_F(UpdateHandlerTest, SendFileHandlerPassWithRetries)
{
    std::string firmwareImage = "image.bin";

    std::uint16_t supported =
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::lpc) |
        static_cast<std::uint16_t>(
            ipmi_flash::FirmwareFlags::UpdateFlags::openWrite);

    EXPECT_CALL(handlerMock, supportedType())
        .WillRepeatedly(Return(ipmi_flash::FirmwareFlags::UpdateFlags::lpc));

    EXPECT_CALL(blobMock, openBlob(ipmi_flash::staticLayoutBlobId, supported))
        .WillRepeatedly(Return(session));

    EXPECT_CALL(handlerMock, sendContents(firmwareImage, session))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));

    EXPECT_CALL(blobMock, closeBlob(session)).Times(3);

    updater.sendFile(ipmi_flash::staticLayoutBlobId, firmwareImage);
}

TEST_F(UpdateHandlerTest, VerifyFileHandleReturnsTrueOnSuccess)
{
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::verifyBlobId, _))
        .WillOnce(Return(session));
    EXPECT_CALL(blobMock, commit(session, _)).WillOnce(Return());
    ipmiblob::StatResponse verificationResponse = {};
    /* the other details of the response are ignored, and should be. */
    verificationResponse.metadata.push_back(
        static_cast<std::uint8_t>(ipmi_flash::ActionStatus::success));

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(verificationResponse));
    EXPECT_CALL(blobMock, closeBlob(session)).WillOnce(Return());

    EXPECT_TRUE(updater.verifyFile(ipmi_flash::verifyBlobId, false));
}

TEST_F(UpdateHandlerTest, VerifyFileHandleSkipsPollingIfIgnoreStatus)
{
    /* if ignoreStatus, it'll skip polling for a verification result. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::verifyBlobId, _))
        .WillOnce(Return(session));
    EXPECT_CALL(blobMock, commit(session, _)).WillOnce(Return());

    EXPECT_CALL(blobMock, closeBlob(session)).WillOnce(Return());

    EXPECT_TRUE(updater.verifyFile(ipmi_flash::verifyBlobId, true));
}

TEST_F(UpdateHandlerTest, VerifyFileConvertsOpenBlobExceptionToToolException)
{
    /* On open, it can except and this is converted to a ToolException. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::verifyBlobId, _))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));
    EXPECT_THROW(updater.verifyFile(ipmi_flash::verifyBlobId, false),
                 ToolException);
}

TEST_F(UpdateHandlerTest, VerifyFileToolException)
{
    /* On open, it can except and this is converted to a ToolException. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::verifyBlobId, _))
        .Times(3)
        .WillRepeatedly(Throw(ToolException("asdf")));
    EXPECT_THROW(updater.verifyFile(ipmi_flash::verifyBlobId, false),
                 ToolException);
}

TEST_F(UpdateHandlerTest, VerifyFileHandleReturnsTrueOnSuccessWithRetries)
{
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::verifyBlobId, _))
        .Times(3)
        .WillRepeatedly(Return(session));
    EXPECT_CALL(blobMock, commit(session, _))
        .WillOnce(Throw(ToolException("asdf")))
        .WillOnce(Throw(ToolException("asdf")))
        .WillOnce(Return());
    ipmiblob::StatResponse verificationResponse = {};
    /* the other details of the response are ignored, and should be. */
    verificationResponse.metadata.push_back(
        static_cast<std::uint8_t>(ipmi_flash::ActionStatus::success));

    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(verificationResponse));
    EXPECT_CALL(blobMock, closeBlob(session)).Times(3).WillRepeatedly(Return());

    EXPECT_TRUE(updater.verifyFile(ipmi_flash::verifyBlobId, false));
}

TEST_F(UpdateHandlerTest, VerifyFileCommitExceptionForwards)
{
    /* On commit, it can except. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::verifyBlobId, _))
        .WillOnce(Return(session));
    EXPECT_CALL(blobMock, commit(session, _))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));
    EXPECT_THROW(updater.verifyFile(ipmi_flash::verifyBlobId, false),
                 ToolException);
}

TEST_F(UpdateHandlerTest, ReadVerisonReturnExpected)
{
    /* It can return as expected, when polling and readBytes succeeds. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::biosVersionBlobId, _))
        .WillOnce(Return(session));
    ipmiblob::StatResponse readVersionResponse = {};
    readVersionResponse.blob_state =
        blobs::StateFlags::open_read | blobs::StateFlags::committed;
    readVersionResponse.size = 10;
    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(readVersionResponse));
    std::vector<uint8_t> resp = {0x2d, 0xfe};
    EXPECT_CALL(blobMock, readBytes(session, 0, _)).WillOnce(Return(resp));

    EXPECT_CALL(blobMock, closeBlob(session)).WillOnce(Return());
    EXPECT_EQ(resp, updater.readVersion(ipmi_flash::biosVersionBlobId));
}

TEST_F(UpdateHandlerTest, ReadVersionExceptionWhenPollingSucceedsReadBytesFails)
{
    /* On readBytes, it can except. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::biosVersionBlobId, _))
        .WillOnce(Return(session));
    ipmiblob::StatResponse readVersionResponse = {};
    readVersionResponse.blob_state =
        blobs::StateFlags::open_read | blobs::StateFlags::committed;
    readVersionResponse.size = 10;
    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .WillOnce(Return(readVersionResponse));
    EXPECT_CALL(blobMock, readBytes(session, 0, _))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));
    EXPECT_CALL(blobMock, closeBlob(session)).WillOnce(Return());
    EXPECT_THROW(updater.readVersion(ipmi_flash::biosVersionBlobId),
                 ToolException);
}

TEST_F(UpdateHandlerTest, ReadVersionReturnsErrorIfPollingFails)
{
    /* It can throw an error, when polling fails. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::biosVersionBlobId, _))
        .Times(3)
        .WillRepeatedly(Return(session));
    ipmiblob::StatResponse readVersionResponse = {};
    readVersionResponse.blob_state = blobs::StateFlags::commit_error;
    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .Times(3)
        .WillRepeatedly(Return(readVersionResponse));
    EXPECT_CALL(blobMock, closeBlob(session)).Times(3).WillRepeatedly(Return());
    EXPECT_THROW(updater.readVersion(ipmi_flash::biosVersionBlobId),
                 ToolException);
}

TEST_F(UpdateHandlerTest, ReadVersionReturnExpectedWithRetries)
{
    /* It can return as expected, when polling and readBytes succeeds. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::biosVersionBlobId, _))
        .Times(3)
        .WillRepeatedly(Return(session));
    ipmiblob::StatResponse readVersionResponse = {};
    readVersionResponse.blob_state =
        blobs::StateFlags::open_read | blobs::StateFlags::committed;
    readVersionResponse.size = 10;
    EXPECT_CALL(blobMock, getStat(TypedEq<std::uint16_t>(session)))
        .Times(3)
        .WillRepeatedly(Return(readVersionResponse));
    std::vector<uint8_t> resp = {0x2d, 0xfe};
    EXPECT_CALL(blobMock, readBytes(session, 0, _))
        .WillOnce(Throw(ToolException("asdf")))
        .WillOnce(Throw(ToolException("asdf")))
        .WillOnce(Return(resp));

    EXPECT_CALL(blobMock, closeBlob(session)).Times(3).WillRepeatedly(Return());
    EXPECT_EQ(resp, updater.readVersion(ipmi_flash::biosVersionBlobId));
}

TEST_F(UpdateHandlerTest, ReadVersionCovertsOpenBlobExceptionToToolException)
{
    /* On open, it can except and this is converted to a ToolException. */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::biosVersionBlobId, _))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));
    EXPECT_THROW(updater.readVersion(ipmi_flash::biosVersionBlobId),
                 ToolException);
}

TEST_F(UpdateHandlerTest, CleanArtifactsSkipsCleanupIfUnableToOpen)
{
    /* It only tries to commit if it's able to open the blob.  However, if
     * committing fails, this error is ignored.
     */
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::cleanupBlobId, _))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));
    EXPECT_CALL(blobMock, commit(_, _)).Times(0);
    EXPECT_CALL(blobMock, closeBlob(_)).Times(0);

    updater.cleanArtifacts();
}

TEST_F(UpdateHandlerTest, CleanArtifactsIfOpenDoesClose)
{
    /* The closeBlob call is called even if commit excepts. */
    std::uint16_t session = 0xa5eb;
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::cleanupBlobId, _))
        .WillOnce(Return(session));
    EXPECT_CALL(blobMock, commit(session, _))
        .WillOnce(Throw(ipmiblob::BlobException("asdf")));
    EXPECT_CALL(blobMock, closeBlob(session));

    updater.cleanArtifacts();
}

TEST_F(UpdateHandlerTest, CleanArtifactsSuccessPath)
{
    std::uint16_t session = 0xa5eb;
    EXPECT_CALL(blobMock, openBlob(ipmi_flash::cleanupBlobId, _))
        .WillOnce(Return(session));
    EXPECT_CALL(blobMock, commit(session, _));
    EXPECT_CALL(blobMock, closeBlob(session));

    updater.cleanArtifacts();
}

class UpdaterTest : public ::testing::Test
{
  protected:
    static constexpr char image[] = "image.bin";
    static constexpr char signature[] = "signature.bin";
    static constexpr char layout[] = "static";
    static constexpr char path[] = "/flash/static";

    ipmiblob::BlobInterfaceMock blobMock;
    std::uint16_t session = 0xbeef;
    bool defaultIgnore = false;
};

TEST_F(UpdaterTest, UpdateMainReturnsSuccessIfAllSuccess)
{
    UpdateHandlerMock handler;

    EXPECT_CALL(handler, checkAvailable(path)).WillOnce(Return(true));
    EXPECT_CALL(handler, sendFile(path, image)).WillOnce(Return());
    EXPECT_CALL(handler, sendFile(ipmi_flash::hashBlobId, signature))
        .WillOnce(Return());
    EXPECT_CALL(handler, verifyFile(ipmi_flash::verifyBlobId, defaultIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(handler, verifyFile(ipmi_flash::updateBlobId, defaultIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(Return(std::vector<std::string>({})));

    updaterMain(&handler, &blobMock, image, signature, layout, defaultIgnore);
}

TEST_F(UpdaterTest, UpdateMainReturnsSuccessIfAllSuccessWithDeleteActiveBlob)
{
    UpdateHandlerMock handler;

    EXPECT_CALL(handler, checkAvailable(path)).WillOnce(Return(true));
    EXPECT_CALL(handler, sendFile(path, image)).WillOnce(Return());
    EXPECT_CALL(handler, sendFile(ipmi_flash::hashBlobId, signature))
        .WillOnce(Return());
    EXPECT_CALL(handler, verifyFile(ipmi_flash::verifyBlobId, defaultIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(handler, verifyFile(ipmi_flash::updateBlobId, defaultIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(handler, cleanArtifacts()).WillOnce(Return());
    EXPECT_CALL(blobMock, deleteBlob(ipmi_flash::activeImageBlobId))
        .WillOnce(Return(true));
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(Return(std::vector<std::string>(
            {ipmi_flash::staticLayoutBlobId, ipmi_flash::activeImageBlobId})));

    updaterMain(&handler, &blobMock, image, signature, layout, defaultIgnore);
}

TEST_F(UpdaterTest, UpdateMainReturnsSuccessWithIgnoreUpdate)
{
    UpdateHandlerMock handler;
    bool updateIgnore = true;

    EXPECT_CALL(handler, checkAvailable(path)).WillOnce(Return(true));
    EXPECT_CALL(handler, sendFile(path, image)).WillOnce(Return());
    EXPECT_CALL(handler, sendFile(ipmi_flash::hashBlobId, signature))
        .WillOnce(Return());
    EXPECT_CALL(handler, verifyFile(ipmi_flash::verifyBlobId, defaultIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(handler, verifyFile(ipmi_flash::updateBlobId, updateIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(Return(std::vector<std::string>({})));

    updaterMain(&handler, &blobMock, image, signature, layout, updateIgnore);
}

TEST_F(UpdaterTest, UpdateMainCleansUpOnFailure)
{
    UpdateHandlerMock handler;

    EXPECT_CALL(handler, checkAvailable(path)).WillOnce(Return(true));
    EXPECT_CALL(handler, sendFile(path, image)).WillOnce(Return());
    EXPECT_CALL(handler, sendFile(ipmi_flash::hashBlobId, signature))
        .WillOnce(Return());
    EXPECT_CALL(handler, verifyFile(ipmi_flash::verifyBlobId, defaultIgnore))
        .WillOnce(Return(false));
    EXPECT_CALL(handler, cleanArtifacts()).WillOnce(Return());
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(Return(std::vector<std::string>({})));

    EXPECT_THROW(updaterMain(&handler, &blobMock, image, signature, layout,
                             defaultIgnore),
                 ToolException);
}

TEST_F(UpdaterTest, UpdateMainExceptsOnUpdateBlobFailure)
{
    UpdateHandlerMock handler;

    EXPECT_CALL(handler, checkAvailable(path)).WillOnce(Return(true));
    EXPECT_CALL(handler, sendFile(path, image)).WillOnce(Return());
    EXPECT_CALL(handler, sendFile(ipmi_flash::hashBlobId, signature))
        .WillOnce(Return());
    EXPECT_CALL(handler, verifyFile(ipmi_flash::verifyBlobId, defaultIgnore))
        .WillOnce(Return(true));
    EXPECT_CALL(handler, verifyFile(ipmi_flash::updateBlobId, defaultIgnore))
        .WillOnce(Return(false));
    EXPECT_CALL(handler, cleanArtifacts()).WillOnce(Return());
    EXPECT_CALL(blobMock, getBlobList())
        .WillOnce(Return(std::vector<std::string>({})));

    EXPECT_THROW(updaterMain(&handler, &blobMock, image, signature, layout,
                             defaultIgnore),
                 ToolException);
}

TEST_F(UpdaterTest, UpdateMainExceptsIfAvailableNotFound)
{
    UpdateHandlerMock handler;

    EXPECT_CALL(handler, checkAvailable(path)).WillOnce(Return(false));

    EXPECT_THROW(updaterMain(&handler, &blobMock, image, signature, layout,
                             defaultIgnore),
                 ToolException);
}

} // namespace host_tool
