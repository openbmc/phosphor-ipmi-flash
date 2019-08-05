#pragma once

#include "status.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>

namespace ipmi_flash
{

/**
 * Implements the update interface by simply triggering a systemd unit.
 */
class SystemdUpdateMechanism : public TriggerableActionInterface
{
  public:
    static std::unique_ptr<TriggerableActionInterface>
        CreateSystemdUpdate(sdbusplus::bus::bus&& bus,
                            const std::string& target,
                            const std::string& mode);

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

    bool trigger() override;
    void abort() override;
    ActionStatus status() override;

  private:
    sdbusplus::bus::bus bus;
    const std::string target;
    const std::string mode;
};

} // namespace ipmi_flash
