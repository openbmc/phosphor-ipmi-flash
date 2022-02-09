#include "image_mock.hpp"

namespace ipmi_flash
{
std::unique_ptr<ImageHandlerMock> CreateImageMock()
{
    return std::make_unique<ImageHandlerMock>();
}
} // namespace ipmi_flash
