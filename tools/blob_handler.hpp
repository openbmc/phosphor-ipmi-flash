#pragma once

#include "blob_interface.hpp"
#include "ipmi_interface.hpp"

class BlobHandler : public BlobInterface
{
  public:
    explicit BlobHandler(IpmiInterface* ipmi) : ipmi(ipmi){};

    /**
     * Retrieve the blob count.
     *
     * @return the number of blob_ids found (0 on failure).
     */
    int getBlobCount();

    /**
     * Given an index into the list of blobs, return the name.
     *
     * @param[in] index - the index into the list of blob ids.
     * @return the name as a string or empty on failure.
     */
    std::string enumerateBlob(int index);

    std::vector<std::string> getBlobList() override;

  private:
    IpmiInterface* ipmi;
};
