#pragma once

#include "update.hpp"
#include "update_systemd.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>

namespace ipmi_flash
{

/**
 * Implements the update interface by simply triggering a reboot via the systemd
 * reboot target.
 */
class RebootUpdateMechanism : public SystemdUpdateMechanism
{
  public:
    static std::unique_ptr<UpdateInterface>
        CreateRebootUpdate(sdbusplus::bus::bus&& bus);

    explicit RebootUpdateMechanism(sdbusplus::bus::bus&& bus) :
        SystemdUpdateMechanism(std::move(bus), "reboot.target",
                               "replace-irreversibly")
    {
    }

    ~RebootUpdateMechanism() = default;
    RebootUpdateMechanism(const RebootUpdateMechanism&) = delete;
    RebootUpdateMechanism& operator=(const RebootUpdateMechanism&) = delete;
    RebootUpdateMechanism(RebootUpdateMechanism&&) = default;
    RebootUpdateMechanism& operator=(RebootUpdateMechanism&&) = default;

    void abortUpdate() override;
    UpdateStatus status() override;
};

} // namespace ipmi_flash
