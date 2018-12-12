#include "blob_handler.hpp"
#include "ipmi_interface_mock.hpp"

#include <gtest/gtest.h>

TEST(BlobHandler, getCountIpmiHappy)
{
    /* Verify returns the value specified by the IPMI response. */

    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);

    EXPECT_EQ(0, blob.getBlobCount());
}

TEST(BlobHandler, getCountIpmiFailure)
{
    /* Verify returns 0 on IPMI failure. */
}
