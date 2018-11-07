#pragma once

#include <functional>
#include <memory>
#include <string>

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
};

struct HandlerPack
{
    std::string blobName;
    ImageHandlerInterface* handler;
};

} // namespace blobs
