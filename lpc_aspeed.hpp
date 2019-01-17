#pragma once

#include "internal/sys.hpp"
#include "window_hw_interface.hpp"

#include <memory>

namespace blobs
{

class LpcMapperAspeed : public HardwareMapperInterface
{
  public:
    static std::unique_ptr<HardwareMapperInterface>
        createAspeedMapper(std::size_t regionSize);

    LpcMapperAspeed(std::size_t regionSize,
                    const internal::Sys* sys = &internal::sys_impl) :
        regionSize(regionSize),
        sys(sys){};

    std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) override;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;

  private:
    std::size_t regionSize;
    const internal::Sys* sys;
};

} // namespace blobs
