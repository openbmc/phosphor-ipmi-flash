#include "triggerable_mock.hpp"

#include <memory>

namespace ipmi_flash
{
std::unique_ptr<TriggerableActionInterface> CreateTriggerMock()
{
    return std::make_unique<TriggerMock>();
}

} // namespace ipmi_flash
