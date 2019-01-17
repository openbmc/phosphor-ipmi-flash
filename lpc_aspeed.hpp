#pragma once

#include "internal/sys.hpp"
#include "window_hw_interface.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace blobs
{

class LpcMapperAspeed : public HardwareMapperInterface
{
  public:
    static std::unique_ptr<HardwareMapperInterface>
        createAspeedMapper(std::uint32_t regionAddress, std::size_t regionSize);

    LpcMapperAspeed(std::uint32_t regionAddress, std::size_t regionSize,
                    const internal::Sys* sys = &internal::sys_impl) :
        regionAddress(regionAddress),
        regionSize(regionSize), sys(sys){};

    std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) override;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;

  private:
    static const std::string lpcControlPath;
    int mappedFd = -1;
    std::uint32_t regionAddress;
    std::size_t regionSize;
    const internal::Sys* sys;
};

} // namespace blobs
