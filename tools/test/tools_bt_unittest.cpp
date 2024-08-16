#include "bt.hpp"
#include "internal_sys_mock.hpp"
#include "progress_mock.hpp"

#include <ipmiblob/blob_errors.hpp>
#include <ipmiblob/test/blob_interface_mock.hpp>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace host_tool
{
namespace
{

using ::testing::_;
using ::testing::ContainerEq;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::Throw;

class BtHandlerTest : public ::testing::Test
{
  protected:
    static constexpr std::uint16_t session = 0xbeef;

    BtHandlerTest() :
        handler(std::make_unique<BtDataHandler>(&blobMock, &progMock, &sysMock))
    {}

    internal::InternalSysMock sysMock;
    ipmiblob::BlobInterfaceMock blobMock;
    ProgressMock progMock;
    std::unique_ptr<BtDataHandler> handler;
    const std::string filePath = "/asdf";
};

TEST_F(BtHandlerTest, verifySendsFileContents)
{
    /* In this very basic test, we'll feed the bt handler data from the internal
     * syscall mock and catch the writes via the blob mock.
     */
    int fd = 1;
    std::vector<std::uint8_t> bytes = {'1', '2', '3', '4'};
    const int fakeFileSize = 100;

    EXPECT_CALL(sysMock, open(Eq(filePath), _)).WillOnce(Return(fd));
    EXPECT_CALL(sysMock, getSize(Eq(filePath))).WillOnce(Return(fakeFileSize));

    EXPECT_CALL(progMock, start(fakeFileSize));

    EXPECT_CALL(sysMock, read(fd, NotNull(), _))
        .WillOnce(Invoke([&](int, void* buf, std::size_t count) {
            EXPECT_TRUE(count > bytes.size());
            std::memcpy(buf, bytes.data(), bytes.size());
            return bytes.size();
        }))
        .WillOnce(Return(0));

    EXPECT_CALL(progMock, updateProgress(bytes.size()));

    EXPECT_CALL(sysMock, close(fd)).WillOnce(Return(0));

    EXPECT_CALL(blobMock, writeBytes(session, 0, ContainerEq(bytes)));

    EXPECT_TRUE(handler->sendContents(filePath, session));
}

TEST_F(BtHandlerTest, sendContentsFailsToOpenFile)
{
    /* If the file doesn't exist or the permissions are wrong, the sendContents
     * will return failure.
     */
    EXPECT_CALL(sysMock, open(Eq(filePath), _)).WillOnce(Return(-1));

    EXPECT_FALSE(handler->sendContents(filePath, session));
}

TEST_F(BtHandlerTest, sendContentsFailsWhenBlobHandlerThrows)
{
    /* The blob handler throws an exception which is caught, the file is closed,
     * and false is returned.
     */

    int fd = 1;
    std::vector<std::uint8_t> bytes = {'1', '2', '3', '4'};
    const int fakeFileSize = 100;

    EXPECT_CALL(sysMock, open(Eq(filePath), _)).WillOnce(Return(fd));
    EXPECT_CALL(sysMock, getSize(Eq(filePath))).WillOnce(Return(fakeFileSize));

    EXPECT_CALL(progMock, start(fakeFileSize));

    EXPECT_CALL(sysMock, read(fd, NotNull(), _))
        .WillOnce(Invoke([&](int, void* buf, std::size_t count) {
            EXPECT_TRUE(count > bytes.size());
            std::memcpy(buf, bytes.data(), bytes.size());
            return bytes.size();
        }));

    EXPECT_CALL(blobMock, writeBytes(session, 0, ContainerEq(bytes)))
        .WillOnce(Throw(ipmiblob::BlobException("failure")));

    EXPECT_CALL(sysMock, close(fd)).WillOnce(Return(0));

    EXPECT_FALSE(handler->sendContents(filePath, session));
}

} // namespace
} // namespace host_tool
