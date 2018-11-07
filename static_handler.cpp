#include "static_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace blobs
{

std::unique_ptr<ImageHandlerInterface>
    StaticLayoutHandler::createStaticLayoutHandler(const std::string& path,
                                                   std::uint32_t flashSize)
{
    return nullptr;
}
} // namespace blobs
