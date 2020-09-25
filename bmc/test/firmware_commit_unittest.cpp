#include "data_mock.hpp"
#include "firmware_handler.hpp"
#include "flags.hpp"
#include "image_mock.hpp"
#include "triggerable_mock.hpp"
#include "util.hpp"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{
using ::testing::_;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::StrictMock;

class FirmwareHandlerCommitTest : public ::testing::Test
{
  protected:
    ImageHandlerMock *imageMock1, *imageMock2;
    std::vector<HandlerPack> blobs;
    std::vector<DataHandlerPack> data;

    void SetUp() override
    {
        std::unique_ptr<ImageHandlerInterface> image =
            std::make_unique<ImageHandlerMock>();
        imageMock1 = reinterpret_cast<ImageHandlerMock*>(image.get());
        blobs.emplace_back(hashBlobId, std::move(image));

        image = std::make_unique<ImageHandlerMock>();
        imageMock2 = reinterpret_cast<ImageHandlerMock*>(image.get());
        blobs.emplace_back("asdf", std::move(image));

        data.emplace_back(FirmwareFlags::UpdateFlags::ipmi, nullptr);
    }
};

TEST_F(FirmwareHandlerCommitTest, VerifyCannotCommitOnFlashImage)
{
    /* Verify the flash image returns failure on this command.  It's a fairly
     * artificial test.
     */
    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::move(data), std::move(CreateActionMap("asdf")));

    EXPECT_CALL(*imageMock2, open("asdf")).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareFlags::UpdateFlags::ipmi, "asdf"));

    EXPECT_FALSE(handler->commit(0, {}));
}

TEST_F(FirmwareHandlerCommitTest, VerifyCannotCommitOnHashFile)
{
    /* Verify the hash file returns failure on this command.  It's a fairly
     * artificial test.
     */
    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler(
        std::move(blobs), std::move(data), std::move(CreateActionMap("asdf")));

    EXPECT_CALL(*imageMock1, open(StrEq(hashBlobId))).WillOnce(Return(true));

    EXPECT_TRUE(handler->open(
        0, blobs::OpenFlags::write | FirmwareFlags::UpdateFlags::ipmi,
        hashBlobId));

    EXPECT_FALSE(handler->commit(0, {}));
}

} // namespace
} // namespace ipmi_flash
