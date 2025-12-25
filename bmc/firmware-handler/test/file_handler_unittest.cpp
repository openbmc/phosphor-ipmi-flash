#include "file_handler.hpp"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{

static constexpr auto TESTPATH = "test.output";

class FileHandlerOpenTest : public ::testing::Test
{
  protected:
    void TearDown() override
    {
        (void)std::remove(TESTPATH);
    }
};

TEST_F(FileHandlerOpenTest, VerifyDefaultOpenIsHappy)
{
    /* Opening a fail may create it? */

    FileHandler handler(TESTPATH);
    EXPECT_TRUE(handler.open(""));
    EXPECT_TRUE(handler.open(""));
}

TEST_F(FileHandlerOpenTest, VerifyOpenForReadFailsWithNoFile)
{
    FileHandler handler(TESTPATH);
    EXPECT_EQ(handler.getSize(), 0);
    EXPECT_FALSE(handler.open("", std::ios::in));
    EXPECT_EQ(handler.getSize(), 0);
}

TEST_F(FileHandlerOpenTest, VerifyOpenForReadSucceedsWithFile)
{
    std::ofstream testfile;
    testfile.open(TESTPATH, std::ios::out);
    testfile << "Hello world";
    FileHandler handler(TESTPATH);
    EXPECT_TRUE(handler.open("", std::ios::in));
}

TEST_F(FileHandlerOpenTest, VerifyWriteDataWrites)
{
    /* Verify writing bytes writes them... flushing data can be an issue here,
     * so we close first.
     */
    FileHandler handler(TESTPATH);
    EXPECT_TRUE(handler.open(""));

    std::vector<std::uint8_t> bytes = {0x01, 0x02};
    std::uint32_t offset = 0;

    EXPECT_TRUE(handler.write(offset, bytes));
    handler.close();

    std::ifstream data;
    data.open(TESTPATH, std::ios::binary);
    char expectedBytes[2];
    data.read(&expectedBytes[0], sizeof(expectedBytes));
    EXPECT_EQ(expectedBytes[0], bytes[0]);
    EXPECT_EQ(expectedBytes[1], bytes[1]);
    /* annoyingly the memcmp was failing... but it's the same data. */
}

TEST_F(FileHandlerOpenTest, VerifySimpleRead)
{
    std::ofstream testfile;
    testfile.open(TESTPATH, std::ios::out);
    std::vector<std::uint8_t> testPattern = {0x0, 0x1, 0x2, 0x3, 0x4,
                                             0x5, 0x6, 0x7, 0x8, 0x9};
    testfile.write(reinterpret_cast<const char*>(testPattern.data()),
                   testPattern.size());
    testfile.close();
    FileHandler handler(TESTPATH);
    EXPECT_EQ(handler.getSize(), testPattern.size());
    EXPECT_TRUE(handler.open("", std::ios::in));
    auto result = handler.read(0, 10);
    EXPECT_EQ(handler.getSize(), testPattern.size());

    if (!result)
    {
        GTEST_FAIL() << "expected result to be present";
        return;
    }
    EXPECT_EQ(result->size(), 10);
    EXPECT_EQ(*result, testPattern);
}

TEST_F(FileHandlerOpenTest, VerifyTruncatedAndOffsetReads)
{
    std::ofstream testfile;
    testfile.open(TESTPATH, std::ios::out);
    std::vector<std::uint8_t> testPattern = {0x0, 0x1, 0x2, 0x3, 0x4,
                                             0x5, 0x6, 0x7, 0x8, 0x9};
    std::vector<std::uint8_t> expectedResult(testPattern.begin() + 3,
                                             testPattern.end());

    testfile.write(reinterpret_cast<const char*>(testPattern.data()),
                   testPattern.size());
    testfile.close();
    FileHandler handler(TESTPATH);
    EXPECT_TRUE(handler.open("", std::ios::in));
    auto result = handler.read(3, 10);

    if (!result)
    {
        GTEST_FAIL() << "expected result to be present";
        return;
    }
    EXPECT_EQ(*result, expectedResult);
}

TEST_F(FileHandlerOpenTest, VerifyBadOffsetReadsFail)
{
    std::ofstream testfile;
    testfile.open(TESTPATH, std::ios::out);
    std::vector<std::uint8_t> testPattern = {0x0, 0x1, 0x2, 0x3, 0x4,
                                             0x5, 0x6, 0x7, 0x8, 0x9};
    testfile.write(reinterpret_cast<const char*>(testPattern.data()),
                   testPattern.size());
    testfile.close();
    FileHandler handler(TESTPATH);
    EXPECT_TRUE(handler.open("", std::ios::in));
    auto result = handler.read(11, 10);
    EXPECT_FALSE(result);
}

} // namespace ipmi_flash
