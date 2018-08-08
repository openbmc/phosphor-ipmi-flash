#pragma once

#include <fstream>

// Abstract Base Class for the Data Interface.
// The DataInterface defines the required methods for sending data down through
// a data channel.  This can allow the data to be written anywhere as long as
// the BMC side knows how to read it.  The data can also be sent down over IPMI
// however, it still must follow this interface.
class DataInterface
{
  public:
    virtual ~DataInterface() = default;

    /* Try to send the file data.
     * @param[in] input : File stream containing the data content to be
     * transmitted
     * @param[in] command : The command corresponding to the data.
     */
    virtual void SendData(std::ifstream input, int command) = 0;

    /* Return true if your data is carried outside the IPMI channel. */
    virtual bool External() = 0;
};
