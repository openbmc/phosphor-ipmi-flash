#pragma once

class IpmiHandler : public IpmiInterface
{
  public:
    IpmiHandler() = default;

    int sendPacket(const std::vector<std::uint8_t>& data) override;
};
