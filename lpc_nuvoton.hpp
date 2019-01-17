#pragma once

#include "internal/sys.hpp"
#include "window_hw_interface.hpp"

#include <memory>
#include <vector>

namespace blobs
{

class LpcMapperNuvoton : public HardwareMapperInterface
{
  public:
    static std::unique_ptr<HardwareMapperInterface> createNuvotonMapper();

    /**
     * Create an LpcMapper for Nuvoton.
     *
     * @param[in] a sys call interface pointer.
     * @todo Needs reserved memory region's physical address and size.
     */
    explicit LpcMapperNuvoton(const internal::Sys* sys = &internal::sys_impl) :
        sys(sys){};

    std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) override;

    std::vector<std::uint8_t> copyFrom(std::uint32_t length) override;

  private:
    const internal::Sys* sys;
};

} // namespace blobs
