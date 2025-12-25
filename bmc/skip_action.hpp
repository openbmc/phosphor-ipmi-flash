#pragma once

#include "status.hpp"

#include <memory>

namespace ipmi_flash
{

// This type will just return success upon trigger(), and even before calling
// trigger.
class SkipAction : public TriggerableActionInterface
{
  public:
    static std::unique_ptr<TriggerableActionInterface> CreateSkipAction();

    SkipAction() = default;
    ~SkipAction() = default;

    // Disallow copy and assign.
    SkipAction(const SkipAction&) = delete;
    SkipAction& operator=(const SkipAction&) = delete;
    SkipAction(SkipAction&&) = delete;
    SkipAction& operator=(SkipAction&&) = delete;

    bool trigger() override;
    void abort() override;
    ActionStatus status() override;
};

} // namespace ipmi_flash
