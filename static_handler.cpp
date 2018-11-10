#include "static_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

const std::string StaticLayoutHandler::stagedFilename =
    "/run/initramfs/bmc-image";

bool StaticLayoutHandler::open(const std::string& path)
{
    this->path = path;
    return false;
}

void StaticLayoutHandler::close()
{
    return;
}

bool StaticLayoutHandler::write(std::uint32_t offset,
                                const std::vector<std::uint8_t>& data)
{
    return false;
}

} // namespace blobs
