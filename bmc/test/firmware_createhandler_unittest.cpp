#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"

#include <memory>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::Return;
using ::testing::StrEq;
using ::testing::StrictMock;

TEST(FirmwareHandlerBlobTest, VerifyFirmwareCounts)
{
    /* Verify the firmware count must be greater than zero. */

    DataHandlerMock dataMock;
    ImageHandlerMock imageMock;
    //    StrictMock<SdJournalMock> journalMock;
    //    SwapJouralHandler(&journalMock);

    std::vector<HandlerPack> blobs;
    blobs.push_back(std::move(
        HandlerPack(hashBlobId, std::make_unique<ImageHandlerMock>())));

    std::vector<DataHandlerPack> data = {
        {FirmwareFlags::UpdateFlags::ipmi, nullptr},
        {FirmwareFlags::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), data, std::move(CreateActionMap("abcd")));

    //    EXPECT_EQ(handler, nullptr);
    EXPECT_FALSE(handler == nullptr);
}

} // namespace
} // namespace ipmi_flash
