#pragma once

#include "status.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>

namespace ipmi_flash
{

/**
 * Representation of what is used for triggering an action with systemd and
 * checking the result by reading a file.
 */
class SystemdWithStatusFile : public TriggerableActionInterface
{
  public:
    /**
     * Create a default SystemdWithStatusFile object that uses systemd to
     * trigger the process.
     *
     * @param[in] bus - an sdbusplus handler for a bus to use.
     * @param[in] path - the path to check for verification status.
     * @param[in] service - the systemd service to start to trigger
     * verification.
     * @param[in] mode - the job-mode when starting the systemd Unit.
     */
    static std::unique_ptr<TriggerableActionInterface>
        CreateSystemdWithStatusFile(sdbusplus::bus::bus&& bus,
                                    const std::string& path,
                                    const std::string& service,
                                    const std::string& mode);

    SystemdWithStatusFile(sdbusplus::bus::bus&& bus, const std::string& path,
                          const std::string& service, const std::string& mode) :
        bus(std::move(bus)),
        checkPath(path), triggerService(service), mode(mode)
    {
    }

    ~SystemdWithStatusFile() = default;
    SystemdWithStatusFile(const SystemdWithStatusFile&) = delete;
    SystemdWithStatusFile& operator=(const SystemdWithStatusFile&) = delete;
    SystemdWithStatusFile(SystemdWithStatusFile&&) = default;
    SystemdWithStatusFile& operator=(SystemdWithStatusFile&&) = default;

    bool trigger() override;
    void abort() override;
    ActionStatus status() override;

    const std::string getMode() const;

  private:
    sdbusplus::bus::bus bus;
    const std::string checkPath;
    const std::string triggerService;
    const std::string mode;
};

class SystemdNoFile : public TriggerableActionInterface
{
  public:
    static std::unique_ptr<TriggerableActionInterface>
        CreateSystemdNoFile(sdbusplus::bus::bus&& bus,
                            const std::string& service,
                            const std::string& mode);

    SystemdNoFile(sdbusplus::bus::bus&& bus, const std::string& service,
                  const std::string& mode) :
        bus(std::move(bus)),
        triggerService(service), mode(mode)
    {
    }

    ~SystemdNoFile() = default;
    SystemdNoFile(const SystemdNoFile&) = delete;
    SystemdNoFile& operator=(const SystemdNoFile&) = delete;
    SystemdNoFile(SystemdNoFile&&) = default;
    SystemdNoFile& operator=(SystemdNoFile&&) = default;

    bool trigger() override;
    void abort() override;
    ActionStatus status() override;

    const std::string getMode() const;

  private:
    sdbusplus::bus::bus bus;
    const std::string triggerService;
    const std::string mode;
    ActionStatus state = ActionStatus::unknown;
};

} // namespace ipmi_flash
