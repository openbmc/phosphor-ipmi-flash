#pragma once

#include "status.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>

namespace ipmi_flash
{

class SystemdPreparation : public TriggerableActionInterface
{
  public:
    static std::unique_ptr<TriggerableActionInterface>
        CreatePreparation(sdbusplus::bus::bus&& bus,
                          const std::string& service);

    SystemdPreparation(sdbusplus::bus::bus&& bus, const std::string& service) :
        bus(std::move(bus)), triggerService(service)
    {
    }

    ~SystemdPreparation() = default;
    SystemdPreparation(const SystemdPreparation&) = delete;
    SystemdPreparation& operator=(const SystemdPreparation&) = delete;
    SystemdPreparation(SystemdPreparation&&) = default;
    SystemdPreparation& operator=(SystemdPreparation&&) = default;

    bool trigger() override;
    void abort() override;
    ActionStatus status() override;

  private:
    sdbusplus::bus::bus bus;
    const std::string triggerService;
    ActionStatus state = ActionStatus::unknown;
};

} // namespace ipmi_flash
