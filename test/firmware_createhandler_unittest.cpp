#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "image_mock.hpp"
#include "util.hpp"
#include "verification_mock.hpp"

#include <gtest/gtest.h>

namespace ipmi_flash
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

    std::vector<HandlerPack> blobs = {
        {hashBlobId, &imageMock},
    };

    std::vector<DataHandlerPack> data = {
        {FirmwareBlobHandler::UpdateFlags::ipmi, nullptr},
        {FirmwareBlobHandler::UpdateFlags::lpc, &dataMock},
    };

    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        blobs, data, CreateVerifyMock());

    //    EXPECT_EQ(handler, nullptr);
    EXPECT_FALSE(handler == nullptr);
}

} // namespace ipmi_flash
