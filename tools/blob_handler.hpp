#pragma once

#include "blob_interface.hpp"
#include "ipmi_interface.hpp"

class BlobHandler : public BlobInterface
{
  public:
    explicit BlobHandler(IpmiInterface* ipmi) : ipmi(ipmi){};

    std::vector<std::string> getBlobList() override;

  private:
    IpmiInterface* ipmi;
};
