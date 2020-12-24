#pragma once

#include <function2/function2.hpp>

#include <cstdint>

namespace ipmi_flash
{

/** The status of the update mechanism or the verification mechanism */
enum class ActionStatus : std::uint8_t
{
    running = 0,
    success = 1,
    failed = 2,
    unknown = 3,
};

class TriggerableActionInterface
{
  public:
    using Callback = fu2::unique_function<void(TriggerableActionInterface&)>;

    virtual ~TriggerableActionInterface() = default;

    /**
     * Trigger action.
     *
     * @return true if successfully started, false otherwise.
     */
    virtual bool trigger() = 0;

    /** Abort the action if possible. */
    virtual void abort() = 0;

    /** Check the current state of the action. */
    virtual ActionStatus status() = 0;

    /** Sets the callback that is executed on completion of the trigger. */
    void setCallback(Callback&& cb)
    {
        this->cb = std::move(cb);
    }

  protected:
    Callback cb;
};

} // namespace ipmi_flash
