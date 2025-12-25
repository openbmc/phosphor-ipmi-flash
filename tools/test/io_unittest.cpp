#include "internal_sys_mock.hpp"
#include "io.hpp"

#include <sys/mman.h>

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace host_tool
{
namespace
{

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;

class DevMemTest : public ::testing::Test
{
  protected:
    DevMemTest() : devmem(std::make_unique<DevMemDevice>(&sys_mock)) {}

    internal::InternalSysMock sys_mock;
    std::unique_ptr<DevMemDevice> devmem;
};

TEST_F(DevMemTest, OpenFromReadFails)
{
    EXPECT_CALL(sys_mock, open(_, _)).WillOnce(Return(-1));

    char destination;
    EXPECT_FALSE(devmem->read(/*offset*/ 0, /*length*/ 1, &destination));
}

TEST_F(DevMemTest, MmapFromReadFails)
{
    int fd = 1;
    EXPECT_CALL(sys_mock, open(_, _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, getpagesize()).WillOnce(Return(4096));
    EXPECT_CALL(sys_mock, mmap(0, _, _, _, fd, _)).WillOnce(Return(MAP_FAILED));
    EXPECT_CALL(sys_mock, close(fd));

    char destination;
    EXPECT_FALSE(devmem->read(/*offset*/ 0, /*length*/ 1, &destination));
}

TEST_F(DevMemTest, CopiesOutOfMmap)
{
    int fd = 1;
    char source = 'a';
    EXPECT_CALL(sys_mock, open(_, _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, getpagesize()).WillOnce(Return(4096));
    EXPECT_CALL(sys_mock, mmap(0, _, _, _, fd, _)).WillOnce(Return(&source));
    EXPECT_CALL(sys_mock, munmap(_, _));
    EXPECT_CALL(sys_mock, close(fd));

    char destination = 'b';
    EXPECT_TRUE(devmem->read(/*offset*/ 0, /*length*/ 1, &destination));
    EXPECT_THAT(destination, Eq('a'));
}

TEST_F(DevMemTest, OpenFromWriteFails)
{
    EXPECT_CALL(sys_mock, open(_, _)).WillOnce(Return(-1));

    char source{};
    EXPECT_FALSE(devmem->write(/*offset*/ 0, /*length*/ 1, &source));
}

TEST_F(DevMemTest, MmapFromWriteFails)
{
    int fd = 1;
    EXPECT_CALL(sys_mock, open(_, _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, getpagesize()).WillOnce(Return(4096));
    EXPECT_CALL(sys_mock, mmap(0, _, _, _, fd, _)).WillOnce(Return(MAP_FAILED));
    EXPECT_CALL(sys_mock, close(fd));

    char source{};
    EXPECT_FALSE(devmem->write(/*offset*/ 0, /*length*/ 1, &source));
}

TEST_F(DevMemTest, CopiesIntoMmap)
{
    int fd = 1;
    char destination = 'b';
    EXPECT_CALL(sys_mock, open(_, _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, getpagesize()).WillOnce(Return(4096));
    EXPECT_CALL(sys_mock, mmap(0, _, _, _, fd, _))
        .WillOnce(Return(&destination));
    EXPECT_CALL(sys_mock, munmap(_, _));
    EXPECT_CALL(sys_mock, close(fd));

    char source = 'a';
    EXPECT_TRUE(devmem->write(/*offset*/ 0, /*length*/ 1, &source));
    EXPECT_THAT(destination, Eq('a'));
}

class PpcMemTest : public ::testing::Test
{
  protected:
    static constexpr char path[] = "/dev/fun";

    PpcMemTest() : devmem(std::make_unique<PpcMemDevice>(path, &sys_mock)) {}

    internal::InternalSysMock sys_mock;
    std::unique_ptr<PpcMemDevice> devmem;
};

TEST_F(PpcMemTest, OpenFromReadFails)
{
    EXPECT_CALL(sys_mock, open(StrEq(path), _)).WillOnce(Return(-1));

    char destination = 'b';
    EXPECT_FALSE(devmem->read(/*offset*/ 0, /*length*/ 1, &destination));
}

TEST_F(PpcMemTest, PreadFails)
{
    int fd = 1;
    EXPECT_CALL(sys_mock, open(StrEq(path), _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, pread(fd, _, 1, 0)).WillOnce(Return(-1));

    char destination = 'b';
    EXPECT_FALSE(devmem->read(/*offset*/ 0, /*length*/ 1, &destination));
}

TEST_F(PpcMemTest, PreadReadWorks)
{
    int fd = 1;
    EXPECT_CALL(sys_mock, open(StrEq(path), _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, pread(fd, _, 1, 0)).WillOnce(Return(0));
    EXPECT_CALL(sys_mock, close(fd));

    /* Test does not validate byte is copied because that's part of pread and
     * not the code.
     */
    char destination = 'b';
    EXPECT_TRUE(devmem->read(/*offset*/ 0, /*length*/ 1, &destination));
}

TEST_F(PpcMemTest, OpenFromWriteFails)
{
    EXPECT_CALL(sys_mock, open(StrEq(path), _)).WillOnce(Return(-1));

    char source = 'a';
    EXPECT_FALSE(devmem->write(/*offset*/ 0, /*length*/ 1, &source));
}

TEST_F(PpcMemTest, PwriteFails)
{
    int fd = 1;
    EXPECT_CALL(sys_mock, open(StrEq(path), _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, pwrite(fd, _, 1, 0)).WillOnce(Return(-1));

    char source = 'a';
    EXPECT_FALSE(devmem->write(/*offset*/ 0, /*length*/ 1, &source));
}

TEST_F(PpcMemTest, PwriteWorks)
{
    int fd = 1;
    EXPECT_CALL(sys_mock, open(StrEq(path), _)).WillOnce(Return(fd));
    EXPECT_CALL(sys_mock, pwrite(fd, _, 1, 0)).WillOnce(Return(0));

    /* Test does not validate byte is copied because that's part of pwrite and
     * not the code.
     */
    char source = 'a';
    EXPECT_TRUE(devmem->write(/*offset*/ 0, /*length*/ 1, &source));
}

} // namespace
} // namespace host_tool
