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

#include "updater.hpp"

#include "bt.hpp"
#include "interface.hpp"
#include "lpc.hpp"

#include <algorithm>
#include <memory>

int updaterMain(BlobInterface* blob, const std::string& interface,
                const std::string& imagePath, const std::string& signaturePath)
{
    std::unique_ptr<DataInterface> handler;

    /* Input has already been validated in this case. */
    if (interface == "ipmibt")
    {
        handler = std::make_unique<BtDataHandler>(blob);
    }
    else if (interface == "ipmilpc")
    {
        handler = std::make_unique<LpcDataHandler>(blob);
    }

    if (!handler)
    {
        /* TODO(venture): use a custom exception. */
        std::fprintf(stderr, "Interface %s is unavailable\n",
                     interface.c_str());
        return -1;
    }

    /* TODO(venture): Add optional parameter to specify the flash type, default
     * to legacy for now.
     */
    std::string goalFirmware = "/flash/image";

    std::vector<std::string> blobs = blob->getBlobList();

    auto blobInst = std::find_if(
        blobs.begin(), blobs.end(),
        [&goalFirmware](const auto& iter) { return (goalFirmware == iter); });
    if (blobInst == blobs.end())
    {
        std::fprintf(stderr, "firmware goal not found!\n");
        return -1; /* throw custom exception. */
    }

    /* Get list of blob_ids, check for /flash/image, or /flash/tarball.
     * TODO(venture) the mechanism doesn't care, but the caller of burn_my_bmc
     * will have in mind which they're sending and we need to verify it's
     * available and use it.
     */

    /* Call stat on /flash/image (or /flash/tarball) and check if data interface
     * is supported.
     */

    return 0;
}
