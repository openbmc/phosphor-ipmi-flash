#pragma once

#include "bmc_update_mock.hpp"
#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "verification_mock.hpp"

#include <memory>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::Return;

class IpmiOnlyFirmwareStaticTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        blobs = {
            {hashBlobId, &imageMock},
            {staticLayoutBlobId, &imageMock},
        };

        std::unique_ptr<VerificationInterface> verifyMock =
            std::make_unique<VerificationMock>();
        verifyMockPtr = reinterpret_cast<VerificationMock*>(verifyMock.get());

        std::unique_ptr<UpdateInterface> updateMock =
            std::make_unique<UpdateMock>();
        updateMockPtr = reinterpret_cast<UpdateMock*>(updateMock.get());

        handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
            blobs, data, std::move(verifyMock), std::move(updateMock));
    }

    void expectedState(FirmwareBlobHandler::UpdateState state)
    {
        auto realHandler = dynamic_cast<FirmwareBlobHandler*>(handler.get());
        EXPECT_EQ(state, realHandler->getCurrentState());
    }

    void openToInProgress(const std::string& blobId)
    {
        EXPECT_CALL(imageMock, open(blobId)).WillOnce(Return(true));
        EXPECT_TRUE(handler->open(session, flags, blobId));
        expectedState(FirmwareBlobHandler::UpdateState::uploadInProgress);
    }

    void getToVerificationPending(const std::string& blobId)
    {
        openToInProgress(blobId);

        EXPECT_CALL(imageMock, close()).WillRepeatedly(Return());
        handler->close(session);
        expectedState(FirmwareBlobHandler::UpdateState::verificationPending);
    }

    void getToVerificationStarted(const std::string& blobId)
    {
        getToVerificationPending(blobId);

        EXPECT_TRUE(handler->open(session, flags, verifyBlobId));
        EXPECT_CALL(*verifyMockPtr, triggerVerification())
            .WillOnce(Return(true));

        EXPECT_TRUE(handler->commit(session, {}));
        expectedState(FirmwareBlobHandler::UpdateState::verificationStarted);
    }

    void getToVerificationCompleted(VerifyCheckResponses checkResponse)
    {
        getToVerificationStarted(staticLayoutBlobId);

        EXPECT_CALL(*verifyMockPtr, status()).WillOnce(Return(checkResponse));
        blobs::BlobMeta meta;
        EXPECT_TRUE(handler->stat(session, &meta));
        expectedState(FirmwareBlobHandler::UpdateState::verificationCompleted);
    }

    void getToUpdatePending()
    {
        getToVerificationCompleted(VerifyCheckResponses::success);

        handler->close(session);
        expectedState(FirmwareBlobHandler::UpdateState::updatePending);
    }

    void getToUpdateStarted()
    {
        getToUpdatePending();
        EXPECT_TRUE(handler->open(session, flags, updateBlobId));

        EXPECT_CALL(*updateMockPtr, triggerUpdate()).WillOnce(Return(true));
        EXPECT_TRUE(handler->commit(session, {}));
        expectedState(FirmwareBlobHandler::UpdateState::updateStarted);
    }

    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr}};
    std::unique_ptr<blobs::GenericBlobInterface> handler;
    VerificationMock* verifyMockPtr;
    UpdateMock* updateMockPtr;

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;
};

class IpmiOnlyFirmwareTest : public ::testing::Test
{
  protected:
    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr}};
    std::unique_ptr<blobs::GenericBlobInterface> handler;

    void SetUp() override
    {
        blobs = {
            {hashBlobId, &imageMock},
            {"asdf", &imageMock},
        };
        handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
            blobs, data, CreateVerifyMock(), CreateUpdateMock());
    }
};

class FakeLpcFirmwareTest : public ::testing::Test
{
  protected:
    DataHandlerMock dataMock;
    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data;
    std::unique_ptr<blobs::GenericBlobInterface> handler;

    void SetUp() override
    {
        blobs = {
            {hashBlobId, &imageMock},
            {"asdf", &imageMock},
        };
        data = {
            {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
            {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
        };
        handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
            blobs, data, CreateVerifyMock(), CreateUpdateMock());
    }
};

} // namespace
} // namespace ipmi_flash
