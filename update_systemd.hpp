#pragma once

#include "update.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>

namespace ipmi_flash
{

/**
 * Implements the update interface by simply triggering a systemd unit.
 */
class SystemdUpdateMechanism : public UpdateInterface
{
  public:
    static std::unique_ptr<UpdateInterface>
        CreateSystemdUpdate(sdbusplus::bus::bus&& bus,
                            const std::string& target,
                            const std::string& mode = "");

    SystemdUpdateMechanism(sdbusplus::bus::bus&& bus, const std::string& target,
                           const std::string& mode) :
        bus(std::move(bus)),
        target(target), mode(mode)
    {
    }

    ~SystemdUpdateMechanism() = default;
    SystemdUpdateMechanism(const SystemdUpdateMechanism&) = delete;
    SystemdUpdateMechanism& operator=(const SystemdUpdateMechanism&) = delete;
    SystemdUpdateMechanism(SystemdUpdateMechanism&&) = default;
    SystemdUpdateMechanism& operator=(SystemdUpdateMechanism&&) = default;

    bool triggerUpdate() override;
    void abortUpdate() override;
    UpdateStatus status() override;

  private:
    sdbusplus::bus::bus bus;
    const std::string target;
    const std::string mode;
};

} // namespace ipmi_flash
