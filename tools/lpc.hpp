#pragma once

#include "blob_interface.hpp"
#include "interface.hpp"

class LpcDataHandler : public DataInterface
{
  public:
    explicit LpcDataHandler(BlobInterface* blob) : blob(blob){};

    bool sendContents(const std::string& input, std::uint16_t session) override;

  private:
    BlobInterface* blob;
};
