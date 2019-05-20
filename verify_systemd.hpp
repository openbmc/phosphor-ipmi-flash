#pragma once

#include "status.hpp"
#include "verify.hpp"

#include <memory>
#include <sdbusplus/bus.hpp>
#include <string>

namespace blobs
{

/**
 * Representation of what is used for verification.  Currently, this reduces the
 * chance of error by using an object instead of two strings to control the
 * verification step, however, it leaves room for a future possibility out
 * something wholly configurable.
 */
class SystemdVerification : public VerificationInterface
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
    static std::unique_ptr<VerificationInterface>
        CreateVerification(sdbusplus::bus::bus&& bus, const std::string& path,
                           const std::string& service);

    SystemdVerification(sdbusplus::bus::bus&& bus, const std::string& path,
                        const std::string& service) :
        bus(std::move(bus)),
        checkPath(path), triggerService(service)
    {
    }

    ~SystemdVerification() = default;
    SystemdVerification(const SystemdVerification&) = delete;
    SystemdVerification& operator=(const SystemdVerification&) = delete;
    SystemdVerification(SystemdVerification&&) = default;
    SystemdVerification& operator=(SystemdVerification&&) = default;

    bool triggerVerification() override;
    void abortVerification() override;
    VerifyCheckResponses checkVerificationState() override;

  private:
    sdbusplus::bus::bus bus;
    const std::string checkPath;
    const std::string triggerService;
};
} // namespace blobs
