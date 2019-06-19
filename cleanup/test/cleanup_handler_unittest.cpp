#include "cleanup.hpp"
#include "filesystem_mock.hpp"
#include "util.hpp"

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
    std::vector<std::string> blobs = {"abcd", "efgh"};
    FileSystemMock mock;
    FileCleanupHandler handler{cleanupBlobId, blobs, &mock};
};

TEST_F(CleanupHandlerTest, GetBlobListReturnsExpectedList)
{
    EXPECT_TRUE(handler.canHandleBlob(cleanupBlobId));
    EXPECT_THAT(handler.getBlobIds(),
                UnorderedElementsAreArray({cleanupBlobId}));
}

TEST_F(CleanupHandlerTest, CommitShouldDeleteFiles)
{
    EXPECT_CALL(mock, remove("abcd")).WillOnce(Return());
    EXPECT_CALL(mock, remove("efgh")).WillOnce(Return());

    EXPECT_TRUE(handler.commit(1, {}));
}

} // namespace
} // namespace ipmi_flash
