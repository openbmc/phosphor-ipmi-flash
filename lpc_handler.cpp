/*
 * Copyright 2018 Google Inc.
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

#include "lpc_handler.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

namespace ipmi_flash
{

bool LpcDataHandler::open()
{
    /* For the ASPEED LPC CTRL driver, the ioctl is required to set up the
     * window, with information from writeMeta() below.
     */
    return true;
}

bool LpcDataHandler::close()
{
    mapper->close();

    return setInitializedAndReturn(false);
}

std::vector<std::uint8_t> LpcDataHandler::copyFrom(std::uint32_t length)
{
    /* TODO: implement this -- in an earlier and different version of this that
     * didn't use BLOBs, the region was memory-mapped and the writes to the data
     * were just done directly from the memory-mapped region instead of a
     * copyFrom() first call.  The idea with this change is that we may not be
     * able to get a memory-mapped handle from the driver from which to
     * automatically read data, but rather must perform some ioctl or other
     * access to get the data from the driver.
     */
    if (!initialized)
    {
        /* TODO: Consider designing some exceptions we can catch for when there
         * is an error.
         */
        return {};
    }

    return mapper->copyFrom(length);
}

bool LpcDataHandler::writeMeta(const std::vector<std::uint8_t>& configuration)
{
    struct LpcRegion lpcRegion;

    if (configuration.size() != sizeof(lpcRegion))
    {
        return false;
    }

    std::memcpy(&lpcRegion, configuration.data(), configuration.size());

    std::uint32_t windowOffset;
    std::uint32_t windowSize;

    /* TODO: LpcRegion sanity checking. */

    std::tie(windowOffset, windowSize) =
        mapper->mapWindow(lpcRegion.address, lpcRegion.length);
    if (windowSize == 0)
    {
        /* Failed to map region. */
        return false;
    }

    return setInitializedAndReturn(true);
}

std::vector<std::uint8_t> LpcDataHandler::readMeta()
{
    /* TODO: Implement this call, s.t. with lpc_aspeed or whatever, you can
     * validate the region.
     */
    return {};
}

} // namespace ipmi_flash
