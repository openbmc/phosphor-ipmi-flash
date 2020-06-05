#pragma once

#include "internal/sys.hpp"

extern "C"
{
#include <pciaccess.h>
} // extern "C"

#include <linux/pci_regs.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#ifndef PCI_STD_NUM_BARS
#define PCI_STD_NUM_BARS 6
#endif // !PCI_STD_NUM_BARS

namespace host_tool
{

/* The ASPEED AST2400 & AST2500 have the same VIDDID. */

/**
 * The PciDevice structure is a copy of the information to uniquely identify a
 * PCI device.
 */
struct PciDevice
{
    std::uint16_t vid;
    std::uint16_t did;
    std::uint8_t bus;
    std::uint8_t dev;
    std::uint8_t func;
    pciaddr_t bars[PCI_STD_NUM_BARS];
};

/**
 * The PciFilter structure is a simple mechanism for filtering devices by their
 * vendor and/or device ids.
 */
struct PciFilter
{
    std::uint16_t vid;
    std::uint16_t did;
};

class PciUtilInterface
{
  public:
    virtual ~PciUtilInterface() = default;

    /**
     * Get a list of PCI devices from a system.
     *
     * @param[in] filter - optional filter for the list.
     * @return the list of devices.
     */
    virtual std::vector<PciDevice>
        getPciDevices(std::optional<PciFilter> filter = std::nullopt) = 0;
};

class PciUtilImpl : public PciUtilInterface
{
  public:
    static PciUtilImpl& getInstance()
    {
        static PciUtilImpl instance;
        return instance;
    }

    std::vector<PciDevice>
        getPciDevices(std::optional<PciFilter> filter = std::nullopt) override;

    PciUtilImpl(const PciUtilImpl&) = delete;
    PciUtilImpl& operator=(const PciUtilImpl&) = delete;

  private:
    PciUtilImpl()
    {
        int ret = pci_system_init();
        if (ret)
        {
            throw internal::errnoException("pci_system_init");
        }
    }

    ~PciUtilImpl()
    {
        pci_system_cleanup();
    }
};

} // namespace host_tool
