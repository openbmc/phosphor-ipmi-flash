/**
 * The goal of these tests is to verify opening the ubi tarball changes state
 * as expected and does not regress.
 */
#include "firmware_handler.hpp"
#include "firmware_unittest.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::UnorderedElementsAreArray;

class FirmwareHandlerNotYetStartedUbitTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        blobs = {
            {hashBlobId, &imageMock},
            {ubiTarballBlobId, &imageMock},
        };

        std::unique_ptr<TriggerableActionInterface> verifyMock =
            std::make_unique<TriggerMock>();
        verifyMockPtr = reinterpret_cast<TriggerMock*>(verifyMock.get());

        std::unique_ptr<TriggerableActionInterface> updateMock =
            std::make_unique<TriggerMock>();
        updateMockPtr = reinterpret_cast<TriggerMock*>(updateMock.get());

        std::unique_ptr<ActionPack> actionPack = std::make_unique<ActionPack>();
        actionPack->preparation = CreateTriggerMock();
        actionPack->verification = std::move(verifyMock);
        actionPack->update = std::move(updateMock);

        ActionMap packs;
        packs[ubiTarballBlobId] = std::move(actionPack);

        handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
            blobs, data, std::move(packs));
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

    ImageHandlerMock imageMock;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data = {
        {FirmwareFlags::UpdateFlags::ipmi, nullptr}};
    std::unique_ptr<blobs::GenericBlobInterface> handler;
    TriggerMock* verifyMockPtr;
    TriggerMock* updateMockPtr;

    std::uint16_t session = 1;
    std::uint16_t flags =
        blobs::OpenFlags::write | FirmwareFlags::UpdateFlags::ipmi;
};

TEST_F(FirmwareHandlerNotYetStartedUbitTest,
       OpeningTarballMovesToUploadInProgress)
{
    expectedState(FirmwareBlobHandler::UpdateState::notYetStarted);

    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray({hashBlobId, ubiTarballBlobId}));

    openToInProgress(ubiTarballBlobId);
    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray(
                    {hashBlobId, ubiTarballBlobId, activeImageBlobId}));
}

} // namespace
} // namespace ipmi_flash
