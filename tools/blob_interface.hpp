#pragma once

#include <string>
#include <vector>

class BlobInterface
{

  public:
    virtual ~BlobInterface() = default;

    virtual std::vector<std::string> getBlobList() = 0;
};
