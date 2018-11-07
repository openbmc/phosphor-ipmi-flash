#pragma once

#include "image_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace blobs
{

class StaticLayoutHandler : public ImageHandlerInterface
{
  public:
    static std::unique_ptr<ImageHandlerInterface>
        createStaticLayoutHandler(const std::string& path,
                                  std::uint32_t flashSize);
};

} // namespace blobs
