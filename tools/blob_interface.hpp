#pragma once

#include <string>
#include <vector>

class BlobInterface
{

  public:
    virtual ~BlobInterface() = default;

    /**
     * Get a list of the blob_ids provided by the BMC.
     *
     * @return list of strings, each representing a blob_id returned.
     */
    virtual std::vector<std::string> getBlobList() = 0;
};
