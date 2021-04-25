#pragma once

#include "image_handler.hpp"

#include <memory>

#include <gmock/gmock.h>

namespace ipmi_flash
{

class ImageHandlerMock : public ImageHandlerInterface
{
  public:
    virtual ~ImageHandlerMock() = default;
    MOCK_METHOD(bool, open, (const std::string&, std::ios_base::openmode),
                (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(bool, write, (std::uint32_t, const std::vector<std::uint8_t>&),
                (override));
    MOCK_METHOD(std::optional<std::vector<std::uint8_t>>, read,
                (std::uint32_t, std::uint32_t), (override));
    MOCK_METHOD(int, getSize, (), (override));
};

std::unique_ptr<ImageHandlerMock> CreateImageMock()
{
    return std::make_unique<ImageHandlerMock>();
}

} // namespace ipmi_flash
