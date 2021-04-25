#pragma once

#include "pciaccess.hpp"

#include <gmock/gmock.h>

namespace host_tool
{

class PciAccessMock : public PciAccess
{
  public:
    MOCK_METHOD(struct pci_device_iterator*, pci_id_match_iterator_create,
                (const struct pci_id_match*), (const, override));
    MOCK_METHOD(void, pci_iterator_destroy, (struct pci_device_iterator*),
                (const, override));
    MOCK_METHOD(void, pci_device_enable, (struct pci_device*),
                (const, override));
    MOCK_METHOD(struct pci_device*, pci_device_next,
                (struct pci_device_iterator*), (const, override));
    MOCK_METHOD(int, pci_device_probe, (struct pci_device*), (const, override));
    MOCK_METHOD(int, pci_device_cfg_read_u8,
                (struct pci_device * dev, std::uint8_t* data, pciaddr_t offset),
                (const, override));
    MOCK_METHOD(int, pci_device_cfg_write_u8,
                (struct pci_device * dev, std::uint8_t data, pciaddr_t offset),
                (const, override));
    MOCK_METHOD(int, pci_device_map_range,
                (struct pci_device*, pciaddr_t, pciaddr_t, unsigned, void**),
                (const, override));
    MOCK_METHOD(int, pci_device_unmap_range,
                (struct pci_device*, void*, pciaddr_t), (const, override));
};

} // namespace host_tool
