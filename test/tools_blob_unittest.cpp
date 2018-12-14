#include "blob_handler.hpp"
#include "crc_mock.hpp"
#include "ipmi_interface_mock.hpp"

#include <gtest/gtest.h>

namespace host_tool
{
CrcInterface* crcIntf = nullptr;

std::uint16_t generateCrc(const std::vector<std::uint8_t>& data)
{
    return (crcIntf) ? crcIntf->generateCrc(data) : 0x00;
}

using ::testing::Eq;
using ::testing::Return;

class BlobHandlerTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        crcIntf = &crcMock;
    }

    CrcMock crcMock;
};

TEST_F(BlobHandlerTest, getCountIpmiHappy)
{
    /* Verify returns the value specified by the IPMI response. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobGetCount};

    /* return 1 blob count. */
    std::vector<std::uint8_t> resp = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                      0x01, 0x00, 0x00, 0x00};

    std::vector<std::uint8_t> bytes = {0x01, 0x00, 0x00, 0x00};
    EXPECT_CALL(crcMock, generateCrc(Eq(bytes))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));
    EXPECT_EQ(1, blob.getBlobCount());
}

TEST_F(BlobHandlerTest, enumerateBlobIpmiHappy)
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
                                      'a',  'b',  'c',  'd',  0x00};

    std::vector<std::uint8_t> bytes = {'a', 'b', 'c', 'd', 0x00};
    std::vector<std::uint8_t> reqCrc = {0x01, 0x00, 0x00, 0x00};
    EXPECT_CALL(crcMock, generateCrc(Eq(reqCrc))).WillOnce(Return(0x00));
    EXPECT_CALL(crcMock, generateCrc(Eq(bytes))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));
    EXPECT_STREQ("abcd", blob.enumerateBlob(1).c_str());
}

TEST_F(BlobHandlerTest, enumerateBlobIpmiNoBytes)
{
    /* Simulate a case where the IPMI command returns no data. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobEnumerate,
        0x00, 0x00, 0x01, 0x00,
        0x00, 0x00};

    /* return value. */
    std::vector<std::uint8_t> resp = {};

    std::vector<std::uint8_t> reqCrc = {0x01, 0x00, 0x00, 0x00};
    EXPECT_CALL(crcMock, generateCrc(Eq(reqCrc))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));
    EXPECT_STREQ("", blob.enumerateBlob(1).c_str());
}

TEST_F(BlobHandlerTest, getBlobListIpmiHappy)
{
    /* Verify returns the list built via the above two commands. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);

    std::vector<std::uint8_t> request1 = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobGetCount};

    /* return 1 blob count. */
    std::vector<std::uint8_t> resp1 = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                       0x01, 0x00, 0x00, 0x00};

    std::vector<std::uint8_t> bytes1 = {0x01, 0x00, 0x00, 0x00};
    EXPECT_CALL(crcMock, generateCrc(Eq(bytes1))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request1))).WillOnce(Return(resp1));

    std::vector<std::uint8_t> request2 = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobEnumerate,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00};

    /* return value. */
    std::vector<std::uint8_t> resp2 = {0xcf, 0xc2, 0x00, 0x00, 0x00,
                                       'a',  'b',  'c',  'd',  0x00};

    std::vector<std::uint8_t> reqCrc = {0x00, 0x00, 0x00, 0x00};
    std::vector<std::uint8_t> bytes2 = {'a', 'b', 'c', 'd', 0x00};
    EXPECT_CALL(crcMock, generateCrc(Eq(reqCrc))).WillOnce(Return(0x00));
    EXPECT_CALL(crcMock, generateCrc(Eq(bytes2))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request2))).WillOnce(Return(resp2));

    /* A std::string is not nul-terminated by default. */
    std::vector<std::string> expectedList = {std::string{"abcd"}};

    EXPECT_EQ(expectedList, blob.getBlobList());
}

TEST_F(BlobHandlerTest, getStatWithMetadata)
{
    /* Stat received metadata. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobStat,
        0x00, 0x00, 'a',  'b',
        'c',  'd',  0x00};

    /* return blob_state: 0xffff, size: 0x00, metadata 0x3445 */
    std::vector<std::uint8_t> resp = {0xcf, 0xc2, 0x00, 0x00, 0x00, 0xff, 0xff,
                                      0x00, 0x00, 0x00, 0x00, 0x02, 0x34, 0x45};

    std::vector<std::uint8_t> reqCrc = {'a', 'b', 'c', 'd', 0x00};
    std::vector<std::uint8_t> respCrc = {0xff, 0xff, 0x00, 0x00, 0x00,
                                         0x00, 0x02, 0x34, 0x45};
    EXPECT_CALL(crcMock, generateCrc(Eq(reqCrc))).WillOnce(Return(0x00));
    EXPECT_CALL(crcMock, generateCrc(Eq(respCrc))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));

    auto meta = blob.getStat("abcd");
    EXPECT_EQ(meta.blob_state, 0xffff);
    EXPECT_EQ(meta.size, 0x00);
    std::vector<std::uint8_t> metadata = {0x34, 0x45};
    EXPECT_EQ(metadata, meta.metadata);
}

TEST_F(BlobHandlerTest, getStatNoMetadata)
{
    /* Stat received no metadata. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobStat,
        0x00, 0x00, 'a',  'b',
        'c',  'd',  0x00};

    /* return blob_state: 0xffff, size: 0x00, metadata 0x3445 */
    std::vector<std::uint8_t> resp = {0xcf, 0xc2, 0x00, 0x00, 0x00, 0xff,
                                      0xff, 0x00, 0x00, 0x00, 0x00, 0x00};

    std::vector<std::uint8_t> reqCrc = {'a', 'b', 'c', 'd', 0x00};
    std::vector<std::uint8_t> respCrc = {0xff, 0xff, 0x00, 0x00,
                                         0x00, 0x00, 0x00};

    EXPECT_CALL(crcMock, generateCrc(Eq(reqCrc))).WillOnce(Return(0x00));
    EXPECT_CALL(crcMock, generateCrc(Eq(respCrc))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));

    auto meta = blob.getStat("abcd");
    EXPECT_EQ(meta.blob_state, 0xffff);
    EXPECT_EQ(meta.size, 0x00);
    std::vector<std::uint8_t> metadata = {};
    EXPECT_EQ(metadata, meta.metadata);
}

TEST_F(BlobHandlerTest, openBlobSucceeds)
{
    /* The open blob succeeds. */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);

    std::vector<std::uint8_t> request = {
        0xcf, 0xc2, 0x00, BlobHandler::BlobOEMCommands::bmcBlobOpen,
        0x00, 0x00, 0x02, 0x04,
        'a',  'b',  'c',  'd',
        0x00};

    std::vector<std::uint8_t> resp = {0xcf, 0xc2, 0x00, 0x00, 0x00, 0xfe, 0xed};

    std::vector<std::uint8_t> reqCrc = {0x02, 0x04, 'a', 'b', 'c', 'd', 0x00};
    std::vector<std::uint8_t> respCrc = {0xfe, 0xed};
    EXPECT_CALL(crcMock, generateCrc(Eq(reqCrc))).WillOnce(Return(0x00));
    EXPECT_CALL(crcMock, generateCrc(Eq(respCrc))).WillOnce(Return(0x00));

    EXPECT_CALL(ipmiMock, sendPacket(Eq(request))).WillOnce(Return(resp));

    auto session =
        blob.openBlob("abcd", blobs::FirmwareBlobHandler::UpdateFlags::lpc);
    EXPECT_EQ(0xedfe, session);
}

} // namespace host_tool
