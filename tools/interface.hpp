#pragma once

#include "flags.hpp"
#include "progress.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

namespace host_tool
{

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

    virtual void waitForRetry()
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    /**
     * Return the supported data interface for this.
     *
     * @return the enum value corresponding to the supported type.
     */
    virtual ipmi_flash::FirmwareFlags::UpdateFlags supportedType() const = 0;
};

} // namespace host_tool
