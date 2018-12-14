#pragma once

#include "internal/sys.hpp"
#include "lpc_interface.hpp"

#include <memory>

namespace blobs
{

class LpcMapperAspeed : public LpcMapperInterface
{
  public:
    static std::unique_ptr<LpcMapperInterface>
        createAspeedMapper(size_t regionSize);

    LpcMapperAspeed(size_t regionSize,
                    const internal::Sys* sys = &internal::sys_impl) :
        regionSize(regionSize),
        sys(sys){};

    std::pair<std::uint32_t, std::uint32_t>
        mapWindow(std::uint32_t address, std::uint32_t length) override;

  private:
    size_t regionSize;
    const internal::Sys* sys;
};

} // namespace blobs
