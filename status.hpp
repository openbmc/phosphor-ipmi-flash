#pragma once

#include <cstdint>

namespace ipmi_flash
{

enum class VerifyCheckResponses : std::uint8_t
{
    running = 0,
    success = 1,
    failed = 2,
    other = 3,
};

/** The status of the update mechanism. */
enum class UpdateStatus : std::uint8_t
{
    running = 0,
    success = 1,
    failed = 2,
    unknown = 3,
};

} // namespace ipmi_flash
