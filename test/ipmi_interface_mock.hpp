#pragma once

class IpmiInterfaceMock : public IpmiInterface
{
  public:
    virtual ~IpmiInterfaceMock() = default;
    MOCK_METHOD1(sendPacket, int(const std::vector<std::uint8_t>&));
};
