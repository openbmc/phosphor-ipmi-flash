#pragma once

#include "internal/sys.hpp"
#include "window_hw_interface.hpp"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace ipmi_flash
{

class LpcMapperNuvoton : public HardwareMapperInterface
{
  public:
    static std::unique_ptr<HardwareMapperInterface>
        createNuvotonMapper(std::uint32_t regionAddress);

    /**
     * Create an LpcMapper for Nuvoton.
     *
     * @param[in] regionAddress - where to map the window into BMC memory.
     * @param[in] a sys call interface pointer.
     * @todo Needs reserved memory region's physical address and size.
     */
    LpcMapperNuvoton(std::uint32_t regionAddress,
                     const internal::Sys* sys = &internal::sys_impl) :
        regionAddress(regionAddress),
        sys(sys){};

    void close() override;

    std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) override;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;

  private:
    std::uint32_t regionAddress;
    const internal::Sys* sys;
};

} // namespace ipmi_flash
