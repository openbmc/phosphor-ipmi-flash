#include "hash_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

bool HashFileHandler::open(const std::string& path)
{
    this->path = path;
    return false;
}

bool HashFileHandler::write(std::uint32_t offset,
                            const std::vector<std::uint8_t>& data)
{
    return false;
}

} // namespace blobs
