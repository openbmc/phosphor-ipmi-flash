#pragma once

#include "image_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

class HashFileHandler : public ImageHandlerInterface
{
  public:
    /**
     * Create a HashFileHandler.
     */
    HashFileHandler() = default;

    bool open(const std::string& path) override;

    bool write(std::uint32_t offset,
               const std::vector<std::uint8_t>& data) override;

  private:
    std::string path;
};
