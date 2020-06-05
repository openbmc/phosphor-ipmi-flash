/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pci.hpp"

extern "C"
{
#include <pciaccess.h>
} // extern "C"

#include <linux/pci_regs.h>

#include <cstring>
#include <optional>
#include <vector>

namespace host_tool
{

std::vector<PciDevice>
    PciUtilImpl::getPciDevices(std::optional<PciFilter> filter)
{
    struct pci_id_match match = {PCI_MATCH_ANY, PCI_MATCH_ANY, PCI_MATCH_ANY,
                                 PCI_MATCH_ANY};
    std::vector<PciDevice> results;

    if (filter.has_value())
    {
        match.vendor_id = filter.value().vid;
        match.device_id = filter.value().did;
    }

    auto it = pci_id_match_iterator_create(&match);
    struct pci_device* dev;

    while ((dev = pci_device_next(it)))
    {
        PciDevice item;

        pci_device_probe(dev);

        item.bus = dev->bus;
        item.dev = dev->dev;
        item.func = dev->func;
        item.vid = dev->vendor_id;
        item.did = dev->device_id;

        for (int i = 0; i < PCI_STD_NUM_BARS; i++)
        {
            item.bars[i] = dev->regions[i].base_addr;
        }

        results.push_back(item);
    }

    pci_iterator_destroy(it);
    return results;
}

} // namespace host_tool
