#include "firmware_handler.hpp"

#include <memory>

#include <gtest/gtest.h>

namespace blobs
{
TEST(FirmwareHandlerTest, CreateEmptyListVerifyHasHash)
{
    auto handler = FirmwareBlobHandler::CreateFirmwareBlobHandler({}, 0);
    auto result = handler->getBlobIds();
    EXPECT_EQ(1, result.size());
    EXPECT_STREQ(result.at(0).c_str(), FirmwareBlobHandler::hashBlobID.c_str());
}
} // namespace blobs
