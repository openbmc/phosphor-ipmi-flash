#pragma once

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
     * @return bool - returns true on success.
     */
    virtual bool open() = 0;
};

} // namespace blobs
