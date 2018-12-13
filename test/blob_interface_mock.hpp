#include "blob_interface.hpp"

class BlobInterfaceMock : public BlobInterface
{
  public:
    virtual ~BlobInterfaceMock() = default;
    MOCK_METHOD0(getBlobList, std::vector<std::string>());
    MOCK_METHOD1(getStat, StatResponse(const std::string&));
};
