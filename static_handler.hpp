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
     *
     * @param[in] path - the path passed into the open firmware manager.
     */
    StaticLayoutHandler() = default;

    bool open(const std::string& path) override;

  private:
    std::string path;
};

} // namespace blobs
