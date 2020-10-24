#pragma once

#include "image_handler.hpp"

#include <gmock/gmock.h>

namespace ipmi_flash
{

class ImageHandlerMock : public ImageHandlerInterface
{
  public:
    virtual ~ImageHandlerMock() = default;
    bool open(const std::string& file,
              std::ios_base::openmode mode = std::ios::out)
    {
        return mock_open(file, mode);
    }

    MOCK_METHOD2(mock_open, bool(const std::string&, std::ios_base::openmode));
    MOCK_METHOD0(close, void());
    MOCK_METHOD2(write, bool(std::uint32_t, const std::vector<std::uint8_t>&));
    MOCK_METHOD2(read, std::optional<std::vector<std::uint8_t>>(std::uint32_t,
                                                                std::uint32_t));
    MOCK_METHOD0(getSize, int());
};

} // namespace ipmi_flash
