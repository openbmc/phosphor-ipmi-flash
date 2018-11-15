#pragma once

#include "image_handler.hpp"

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

class HashFileHandler : public ImageHandlerInterface
{
  public:
    /**
     * Create a HashFileHandler.  This object is basically a filewriter.
     *
     * @param[in] hashFilename - file to use for the hash contents, fully
     * qualified file system path.
     */
    explicit HashFileHandler(const std::string& hashFilename) :
        hashFilename(hashFilename)
    {
    }

    bool open(const std::string& path) override;
    void close() override;
    bool write(std::uint32_t offset,
               const std::vector<std::uint8_t>& data) override;

  private:
    /** the active hash path, ignore. */
    std::string path;

    /** The file handle. */
    std::ofstream hashFile;

    /** The filename (including path) to use to write the hash contents. */
    std::string hashFilename;
};

} // namespace blobs
