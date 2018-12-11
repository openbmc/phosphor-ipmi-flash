#pragma once

#include <cstdint>
#include <string>

class DataInterface
{
  public:
    virtual ~DataInterface() = default;

    /**
     * Given an open session to either /flash/image, /flash/tarball, or
     * /flash/hash, this method will configure, and send the data, but not close
     * the session.
     *
     * @param[in] input - path to file to send.
     * @param[in] session - the session ID to use.
     * @return bool on success.
     */
    virtual bool sendContents(const std::string& input,
                              std::uint16_t session) = 0;
};
