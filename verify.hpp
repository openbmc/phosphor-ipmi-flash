#pragma once

#include "status.hpp"

#include <memory>
#include <string>

namespace ipmi_flash
{

class VerificationInterface
{
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

    /** Check the current state of the verification process. */
    virtual ActionStatus status() = 0;
};

} // namespace ipmi_flash
