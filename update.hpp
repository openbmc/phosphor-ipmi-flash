#pragma once

#include "status.hpp"

namespace ipmi_flash
{

class UpdateInterface
{
  public:
    virtual ~UpdateInterface() = default;

    /**
     * Trigger the update mechanism.
     *
     * @return true if successfully started, false otherwise.
     */
    virtual bool triggerUpdate() = 0;

    /** Abort the update process. */
    virtual void abortUpdate() = 0;

    /** Check the current state of the update process. */
    virtual ActionStatus status() = 0;
};

} // namespace ipmi_flash
