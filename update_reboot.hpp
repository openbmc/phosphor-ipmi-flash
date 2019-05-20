#pragma once

#include "update.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>

namespace ipmi_flash
{

/**
 * Implements the update interface by simply triggering a reboot via the systemd
 * reboot target.
 */
class RebootUpdateMechanism : public UpdateInterface
{

  public:
    static std::unique_ptr<UpdateInterface>
        CreateRebootUpdate(sdbusplus::bus::bus&& bus);

    explicit RebootUpdateMechanism(sdbusplus::bus::bus&& bus) :
        bus(std::move(bus))
    {
    }

    ~RebootUpdateMechanism() = default;
    RebootUpdateMechanism(const RebootUpdateMechanism&) = delete;
    RebootUpdateMechanism& operator=(const RebootUpdateMechanism&) = delete;
    RebootUpdateMechanism(RebootUpdateMechanism&&) = default;
    RebootUpdateMechanism& operator=(RebootUpdateMechanism&&) = default;

    bool triggerUpdate() override;
    void abortUpdate() override;
    UpdateStatus status() override;

  private:
    sdbusplus::bus::bus bus;
};

} // namespace ipmi_flash
