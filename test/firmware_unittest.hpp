#pragma once

#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "verification_mock.hpp"

#include <memory>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blobs
{

class IpmiOnlyFirmwareTest : public ::testing::Test
{
  protected:
    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr}};
    std::unique_ptr<GenericBlobInterface> handler;

    void SetUp() override
    {
        blobs = {
            {hashBlobId, &imageMock},
            {"asdf", &imageMock},
        };
        handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
            blobs, data, CreateVerifyMock());
    }
};

class FakeLpcFirmwareTest : public ::testing::Test
{
  protected:
    DataHandlerMock dataMock;
    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data;
    std::unique_ptr<GenericBlobInterface> handler;

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
            blobs, data, CreateVerifyMock());
    }
};

} // namespace blobs
