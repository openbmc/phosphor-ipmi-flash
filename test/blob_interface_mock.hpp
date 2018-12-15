#include "blob_interface.hpp"

#include <gmock/gmock.h>

namespace host_tool
{

class BlobInterfaceMock : public BlobInterface
{
  public:
    virtual ~BlobInterfaceMock() = default;
    MOCK_METHOD0(getBlobList, std::vector<std::string>());
    MOCK_METHOD1(getStat, StatResponse(const std::string&));
    MOCK_METHOD2(openBlob,
                 std::uint16_t(const std::string&,
                               blobs::FirmwareBlobHandler::UpdateFlags));
    MOCK_METHOD(closeBlob, void(std::uint16_t));
};

} // namespace host_tool
