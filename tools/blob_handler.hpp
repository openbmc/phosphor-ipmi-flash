#pragma once

#include "blob_interface.hpp"

class BlobHandler : public BlobInterface
{
  public:
    BlobHandler() = default;

    std::vector<std::string> getBlobList() override;
};
