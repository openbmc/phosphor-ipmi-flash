#include "blob_handler.hpp"
#include "ipmi_interface_mock.hpp"

#include <gtest/gtest.h>

TEST(BlobHandler, getCountIpmiFailure)
{
    /* Verify returns 0 on IPMI failure. */

    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
}

TEST(BlobHandler, getCountIpmiHappy)
{
    /* Verify returns the value specified by the IPMI response. */
}
