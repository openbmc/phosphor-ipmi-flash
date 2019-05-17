#pragma once

#include "config.h"

#include <memory>
#include <string>

#if HAVE_SDBUSPLUS
#include <sdbusplus/bus.hpp>
#else
namespace sdbusplus
{
namespace bus
{
class bus
{
};
} // namespace bus
} // namespace sdbusplus
#endif

namespace blobs
{

class VerificationInterface {
  public:
  virtual ~VerificationInterface() = default;

  /**
   * Trigger verification service.
   *
   * @return true if successfully started, false otherwise.
   */
  virtual bool triggerVerification() = 0;

  /** Abort the verification process. */
  virtual void abortVerification() = 0;
};

/**
 * Representation of what is used for verification.  Currently, this reduces the
 * chance of error by using an object instead of two strings to control the
 * verification step, however, it leaves room for a future possibility out
 * something wholly configurable.
 */
class Verification : public VerificationInterface
{
  public:
    /**
     * Create a default Verification object that uses systemd to trigger the process.
     *
     * @param[in] bus - an sdbusplus handler for a bus to use.
     * @param[in] path - the path to check for verification status.
     * @param[in[ service - the systemd service to start to trigger verification.
     */
    static std::unique_ptr<VerificationInterface> CreateDefaultVerification(sdbusplus::bus::bus&& bus, const std::string& path, const std::string& service);

    Verification(sdbusplus::bus::bus&& bus, const std::string& path, const std::string& service) :
        bus(std::move(bus)), checkPath(path), triggerService(service)
    {
    }

    bool triggerVerification() override;
    void abortVerification() override;

 private:
    sdbusplus::bus::bus bus;
    const std::string checkPath;
    const std::string triggerService;
};

}
