#pragma once

#include "image_handler.hpp"

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

class StaticLayoutHandler : public ImageHandlerInterface
{
  public:
    /**
     * Create a StaticLayoutHandler.
     */
    explicit StaticLayoutHandler(const std::string& temporaryName) :
        stagedFilename(temporaryName){};

    bool open(const std::string& path) override;
    void close() override;
    bool write(std::uint32_t offset,
               const std::vector<std::uint8_t>& data) override;

  private:
    std::string path;

    /** The staged output file stream object. */
    std::ofstream stagedOutput;

    /** The file to use for staging the bytes. */
    std::string stagedFilename;
};

} // namespace blobs
