#include "io.hpp"

namespace host_tool
{

bool DevMemDevice::write(const std::size_t offset, const std::size_t length,
                         const void* const source)
{
    return false;
}

} // namespace host_tool
