#include "blob_handler.hpp"
#include "ipmi_interface_mock.hpp"

#include <gtest/gtest.h>

TEST(BlobTest, GetCountIpmiHappy)
{
    /* Verify that getCount returns the value expected: IPMI is little endian by
     * spec.
     */
    IpmiInterfaceMock ipmiMock;
    BlobHandler blob(&ipmiMock);
}

TEST(BlobTest, GetCountIpmiUnHappy)
{
    /* Return 0 on IPMI response failure. */
}
