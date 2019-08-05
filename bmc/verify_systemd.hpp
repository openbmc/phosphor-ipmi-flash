#pragma once

#include "status.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>

namespace ipmi_flash
{

/**
 * Representation of what is used for verification.  Currently, this reduces the
 * chance of error by using an object instead of two strings to control the
 * verification step, however, it leaves room for a future possibility out
 * something wholly configurable.
 */
class SystemdVerification : public TriggerableActionInterface
{
  public:
    /**
     * Create a default Verification object that uses systemd to trigger the
     * process.
     *
     * @param[in] bus - an sdbusplus handler for a bus to use.
     * @param[in] path - the path to check for verification status.
     * @param[in[ service - the systemd service to start to trigger
     * verification.
     */
    static std::unique_ptr<TriggerableActionInterface>
        CreateVerification(sdbusplus::bus::bus&& bus, const std::string& path,
                           const std::string& service, const std::string& mode);

    SystemdVerification(sdbusplus::bus::bus&& bus, const std::string& path,
                        const std::string& service, const std::string& mode) :
        bus(std::move(bus)),
        checkPath(path), triggerService(service), mode(mode)
    {
    }

    ~SystemdVerification() = default;
    SystemdVerification(const SystemdVerification&) = delete;
    SystemdVerification& operator=(const SystemdVerification&) = delete;
    SystemdVerification(SystemdVerification&&) = default;
    SystemdVerification& operator=(SystemdVerification&&) = default;

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
} // namespace ipmi_flash
