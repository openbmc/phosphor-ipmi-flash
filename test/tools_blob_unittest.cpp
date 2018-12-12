#include "blob_handler.hpp"
#include "ipmi_interface_mock.hpp"

#include <gtest/gtest.h>

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
