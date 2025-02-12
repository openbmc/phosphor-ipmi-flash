#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ipmi_flash
{

/**
 * Each image update mechanism must implement the ImageHandlerInterface.
 */
class ImageHandlerInterface
{
  public:
    virtual ~ImageHandlerInterface() = default;

    /**
     * open the firmware update mechanism.
     *
     * @param[in] path - the path passed to the handler (the blob_id).
     * @return bool - returns true on success.
     */
    virtual bool open(const std::string& path,
                      std::ios_base::openmode mode) = 0;

    /**
     * close the image.
     */
    virtual void close() = 0;

    /**
     * write data to the staged file.
     *
     * @param[in] offset - 0-based offset into the file.
     * @param[in] data - the data to write.
     * @return bool - returns true on success.
     */
    virtual bool write(std::uint32_t offset,
                       const std::vector<std::uint8_t>& data) = 0;
    /**
     * read data from a file.
     *
     * @param[in] offset - 0-based offset into the file.
     * @param[in] size - the number of bytes
     * @return std::optional<std::vector<std::uint8_t>> - returns std::nullopt
     * on failure otherwise returns a vector filled with the bytes read.
     *
     */
    virtual std::optional<std::vector<std::uint8_t>> read(
        std::uint32_t offset, std::uint32_t size) = 0;

    /**
     * return the size of the file (if that notion makes sense).
     *
     * @return the size in bytes of the image staged.
     */
    virtual int getSize() = 0;
};

class HandlerPack
{
  public:
    HandlerPack(const std::string& name,
                std::unique_ptr<ImageHandlerInterface> handler) :
        blobName(name), handler(std::move(handler))
    {}

    HandlerPack() = default;
    ~HandlerPack() = default;
    HandlerPack(const HandlerPack&) = delete;
    HandlerPack& operator=(const HandlerPack&) = delete;
    HandlerPack(HandlerPack&&) = default;
    HandlerPack& operator=(HandlerPack&&) = default;

    std::string blobName;
    std::unique_ptr<ImageHandlerInterface> handler;
};

} // namespace ipmi_flash
