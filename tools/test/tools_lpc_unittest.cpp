#include "internal_sys_mock.hpp"
#include "io_mock.hpp"
#include "lpc.hpp"

#include <cstring>
#include <ipmiblob/test/blob_interface_mock.hpp>

#include <gtest/gtest.h>

namespace host_tool
{

using ::testing::ContainerEq;

TEST(LpcHandleTest, verifySendsFileContents)
{
    internal::InternalSysMock sysMock;
    ipmiblob::BlobInterfaceMock blobMock;
    HostIoInterfaceMock ioMock;

    LpcDataHandler handler(&blobMock, &ioMock, &sysMock);
    std::uint16_t session = 0xbeef;
    std::string filePath = "/asdf";

    LpcRegion host_lpc_buf;
    host_lpc_buf.address = 0xfedc1000;
    host_lpc_buf.length = 0x1000;

    std::vector<std::uint8_t> bytes(sizeof(host_lpc_buf));
    std::memcpy(bytes.data(), &host_lpc_buf, sizeof(host_lpc_buf));

    EXPECT_CALL(blobMock, writeMeta(session, 0, ContainerEq(bytes)));

    EXPECT_FALSE(handler.sendContents(filePath, session));
}

} // namespace host_tool
