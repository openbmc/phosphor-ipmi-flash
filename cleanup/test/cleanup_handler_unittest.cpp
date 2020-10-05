#include "cleanup.hpp"
#include "filesystem_mock.hpp"
#include "util.hpp"

#include <blobs-ipmid/blobs.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

using ::testing::Return;
using ::testing::UnorderedElementsAreArray;

class CleanupHandlerTest : public ::testing::Test
{
  protected:
    CleanupHandlerTest() : mock(std::make_unique<FileSystemMock>())
    {
        mock_ptr = mock.get();
        handler = std::make_unique<FileCleanupHandler>(cleanupBlobId, blobs,
                                                       std::move(mock));
    }

    std::vector<std::string> blobs = {"abcd", "efgh"};
    std::unique_ptr<FileSystemMock> mock;
    FileSystemMock* mock_ptr;
    std::unique_ptr<FileCleanupHandler> handler;
};

TEST_F(CleanupHandlerTest, GetBlobListReturnsExpectedList)
{
    EXPECT_TRUE(handler->canHandleBlob(cleanupBlobId));
    EXPECT_THAT(handler->getBlobIds(),
                UnorderedElementsAreArray({cleanupBlobId}));
}

TEST_F(CleanupHandlerTest, CommitShouldDeleteFiles)
{
    EXPECT_CALL(*mock_ptr, remove("abcd")).WillOnce(Return());
    EXPECT_CALL(*mock_ptr, remove("efgh")).WillOnce(Return());

    EXPECT_TRUE(handler->commit(1, {}));
}

TEST_F(CleanupHandlerTest, VerifyDefaultBlobMethods)
{
    // Test each of the blob handler commands.
    EXPECT_TRUE(handler->open(/*session*/ 0, /*flags*/ 0, "abcd"));
    EXPECT_TRUE(handler->close(/*session*/ 0));
    EXPECT_TRUE(handler->expire(/*session*/ 0));
    EXPECT_FALSE(handler->deleteBlob("abcd"));

    blobs::BlobMeta meta;
    EXPECT_FALSE(handler->stat("abcd", &meta));
    EXPECT_FALSE(handler->stat(/*session*/ 0, &meta));

    EXPECT_THAT(handler->read(/*session*/ 0, /*offset*/ 0, 1),
                ::testing::IsEmpty());

    std::vector<uint8_t> data = {0x01};
    EXPECT_FALSE(handler->write(/*session*/ 0, /*offset*/ 0, data));
}

} // namespace
} // namespace ipmi_flash
