#include "blob_handler.hpp"
#include "ipmi_interface_mock.hpp"

#include <gtest/gtest.h>

std::uint16_t expectedCrc = 0;

std::uint16_t generateCrc(const std::vector<std::uint8_t>& data)
{
    return expectedCrc;
}

using ::testing::Eq;
using ::testing::Return;

TEST(BlobHandler, getCountIpmiHappy)
{
    /* Verify returns the value specified by the IPMI response. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobGetCount};

    /* return 1 blob count. */
    std::vector<std::uint8_t> resp = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                      0x01, 0x00, 0x00, 0x00};

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));
    EXPECT_EQ(1, blob.getBlobCount());
}

TEST(BlobHandler, enumerateBlobIpmiHappy)
{
    /* Verify returns the name specified by the IPMI response. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobEnumerate,
        0x00, 0x00, 0x01, 0x00,
        0x00, 0x00};

    /* return value. */
    std::vector<std::uint8_t> resp = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                      'a',  'b',  'c',  'd'};

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));
    EXPECT_STREQ("abcd", blob.enumerateBlob(1).c_str());
}

TEST(BlobHandler, getBlobListIpmiHappy)
{
    /* Verify returns the list built via the above two commands. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);

    std::vector<std::uint8_t> request1 = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobGetCount};

    /* return 1 blob count. */
    std::vector<std::uint8_t> resp1 = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                       0x01, 0x00, 0x00, 0x00};

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request1))).WillOnce(Return(resp1));

    std::vector<std::uint8_t> request2 = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobEnumerate,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00};

    /* return value. */
    std::vector<std::uint8_t> resp2 = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                       'a',  'b',  'c',  'd'};

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request2))).WillOnce(Return(resp2));

    std::vector<std::string> expectedList = {"abcd"};

    EXPECT_EQ(expectedList, blob.getBlobList());
}
