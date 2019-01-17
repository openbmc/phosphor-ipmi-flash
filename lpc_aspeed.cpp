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

#include "lpc_aspeed.hpp"

#include "window_hw_interface.hpp"

#include <fcntl.h>
#include <linux/aspeed-lpc-ctrl.h>
#include <linux/kernel.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

namespace blobs
{

const std::string LpcMapperAspeed::lpcControlPath = "/dev/aspeed-lpc-ctrl";

std::unique_ptr<HardwareMapperInterface>
    LpcMapperAspeed::createAspeedMapper(std::uint32_t regionAddress,
                                        std::size_t regionSize)
{
    /* NOTE: considered using a joint factory to create one or the other, for
     * now, separate factories.
     */
    return std::make_unique<LpcMapperAspeed>(regionAddress, regionSize);
}

std::pair<std::uint32_t, std::uint32_t>
    LpcMapperAspeed::mapWindow(std::uint32_t address, std::uint32_t length)
{
    static const std::uint32_t MASK_64K = 0xFFFFU;
    const std::uint32_t offset = address & MASK_64K;

    if (offset + length > regionSize)
    {
        std::fprintf(stderr,
                     "requested window size %" PRIu32 ", offset %#" PRIx32
                     " is too large for mem region"
                     " of size %zu\n",
                     length, offset, regionSize);
        /* TODO: need to throw an exception at this point to store the data to
         * provide an EBIG response later.
         */
        /* *windowSize = regionSize - offset; */
        return std::make_pair(0, 0);
    }

    struct aspeed_lpc_ctrl_mapping map = {
        .window_type = ASPEED_LPC_CTRL_WINDOW_MEMORY,
        .window_id = 0,
        .flags = 0,
        .addr = address & ~MASK_64K,
        .offset = 0,
        .size = __ALIGN_KERNEL_MASK(offset + length, MASK_64K),
    };

    std::fprintf(stderr,
                 "requesting Aspeed LPC window at %#" PRIx32 " of size %" PRIu32
                 "\n",
                 map.addr, map.size);

    const auto lpcControlFd = sys->open(lpcControlPath.c_str(), O_RDWR);
    if (lpcControlFd == -1)
    {
        std::fprintf(stderr,
                     "cannot open Aspeed LPC kernel control dev \"%s\"\n",
                     lpcControlPath.c_str());
        return std::make_pair(0, 0);
    }

    if (sys->ioctl(lpcControlFd, ASPEED_LPC_CTRL_IOCTL_MAP, &map) == -1)
    {
        std::fprintf(stderr, "Failed to ioctl Aspeed LPC map with error %s\n",
                     std::strerror(errno));
        sys->close(lpcControlFd);
        return std::make_pair(0, 0);
    }

    sys->close(lpcControlFd);
    return std::make_pair(offset, length);
}

std::vector<std::uint8_t> LpcMapperAspeed::copyFrom(std::uint32_t length)
{
    /* TODO: Implement this. */
    return {};
}

} // namespace blobs
