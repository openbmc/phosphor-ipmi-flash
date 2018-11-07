#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace blobs
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
    virtual bool open(const std::string& path) = 0;

    /**
     * write data to the staged file.
     *
     * @param[in] offset - 0-based offset into the file.
     * @param[in] data - the data to write.
     * @return bool - returns true on success.
     */
    virtual bool write(std::uint32_t offset,
                       const std::vector<std::uint8_t>& data) = 0;
};

struct HandlerPack
{
    std::string blobName;
    ImageHandlerInterface* handler;
};

} // namespace blobs
