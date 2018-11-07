#include "static_handler.hpp"

#include <memory>
#include <string>

namespace blobs
{

bool StaticLayoutHandler::open(const std::string& path)
{
    this->path = path;
    return false;
}

} // namespace blobs
