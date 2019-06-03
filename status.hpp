#pragma once

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

} // namespace ipmi_flash
