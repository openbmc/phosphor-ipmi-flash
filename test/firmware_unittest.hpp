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

    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr}};
    std::unique_ptr<blobs::GenericBlobInterface> handler;
    VerificationMock* verifyMockPtr;
    UpdateMock* updateMockPtr;
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

} // namespace ipmi_flash
