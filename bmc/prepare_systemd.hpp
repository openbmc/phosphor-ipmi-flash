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
        CreatePreparation(sdbusplus::bus::bus&& bus, const std::string& service,
                          const std::string& mode);

    SystemdPreparation(sdbusplus::bus::bus&& bus, const std::string& service,
                       const std::string& mode) :
        bus(std::move(bus)),
        triggerService(service), mode(mode)
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
    const std::string mode;
    ActionStatus state = ActionStatus::unknown;
};

} // namespace ipmi_flash
