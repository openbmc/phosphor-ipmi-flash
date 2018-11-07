#pragma once

#include "image_handler.hpp"

#include <memory>
#include <string>

namespace blobs
{

class StaticLayoutHandler : public ImageHandlerInterface
{
  public:
    /**
     * Create a StaticLayoutHandler.
     */
    StaticLayoutHandler() = default;

    bool open(const std::string& path) override;

  private:
    std::string path;
};

} // namespace blobs
