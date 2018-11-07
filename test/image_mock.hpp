#pragma once

#include "image_handler.hpp"

#include <gmock/gmock.h>

namespace blobs
{

class ImageHandlerMock : public ImageHandlerInterface
{
  public:
    virtual ~ImageHandlerMock() = default;

    MOCK_METHOD1(open, bool(const std::string&));
};

} // namespace blobs
