#pragma once

#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"

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

        std::unique_ptr<TriggerableActionInterface> verifyMock =
            std::make_unique<TriggerMock>();
        verifyMockPtr = reinterpret_cast<TriggerMock*>(verifyMock.get());

        std::unique_ptr<TriggerableActionInterface> updateMock =
            std::make_unique<TriggerMock>();
        updateMockPtr = reinterpret_cast<TriggerMock*>(updateMock.get());

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
        EXPECT_CALL(*verifyMockPtr, trigger()).WillOnce(Return(true));

        EXPECT_TRUE(handler->commit(session, {}));
        expectedState(FirmwareBlobHandler::UpdateState::verificationStarted);
    }

    void getToVerificationCompleted(ActionStatus checkResponse)
    {
        getToVerificationStarted(staticLayoutBlobId);

        EXPECT_CALL(*verifyMockPtr, status()).WillOnce(Return(checkResponse));
        blobs::BlobMeta meta;
        EXPECT_TRUE(handler->stat(session, &meta));
        expectedState(FirmwareBlobHandler::UpdateState::verificationCompleted);
    }

    void getToUpdatePending()
    {
        getToVerificationCompleted(ActionStatus::success);

        handler->close(session);
        expectedState(FirmwareBlobHandler::UpdateState::updatePending);
    }

    void getToUpdateStarted()
    {
        getToUpdatePending();
        EXPECT_TRUE(handler->open(session, flags, updateBlobId));

        EXPECT_CALL(*updateMockPtr, trigger()).WillOnce(Return(true));
        EXPECT_TRUE(handler->commit(session, {}));
        expectedState(FirmwareBlobHandler::UpdateState::updateStarted);
    }

    void getToUpdateCompleted(ActionStatus result)
    {
        getToUpdateStarted();
        EXPECT_CALL(*updateMockPtr, status()).WillOnce(Return(result));

        blobs::BlobMeta meta;
        EXPECT_TRUE(handler->stat(session, &meta));
        expectedState(FirmwareBlobHandler::UpdateState::updateCompleted);
    }

    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr}};
    std::unique_ptr<blobs::GenericBlobInterface> handler;
    TriggerMock* verifyMockPtr;
    TriggerMock* updateMockPtr;

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareBlobHandler::UpdateFlags::ipmi;

    std::vector<std::string> startingBlobs = {staticLayoutBlobId, hashBlobId};
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
            blobs, data, CreateTriggerMock(), CreateTriggerMock());
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
            blobs, data, CreateTriggerMock(), CreateTriggerMock());
    }
};

} // namespace
} // namespace ipmi_flash
