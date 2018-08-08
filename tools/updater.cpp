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

#include <fstream>
#include <memory>
#include <set>
#include <stdexcept>

#include "bt.hpp"
#include "raw.hpp"
#include "updatehelper.hpp"

extern "C" {
#include "ipmitoolintf.h"
} // extern "C"

void UpdaterMain(const std::string& interface, const std::string& image,
                 const std::string& signature)
{
    static const std::set<std::string> supportedInterfaces = {"ipmibt"};

    /* Check if interface is supported. */
    if (!supportedInterfaces.count(interface))
    {
        throw std::runtime_error("Unsupported interface");
    }

    /* NOTE: Presently, the hash signature being separate is optional on the BMC
     * but isn't here, for now.
     */
    std::ifstream imageStream;
    std::ifstream hashStream;

    imageStream.open(image);
    hashStream.open(signature);
    if (imageStream.bad() || hashStream.bad())
    {
        throw std::runtime_error("Unable to open image or hash file.");
    }

    /* There are three key components to the process.  There's the data handler,
     * which deals with sending the data, and it uses a convenience method to
     * package the specific IPMI firmware update commands, and those commands
     * are then routed through a convenience layer that will handle calling into
     * the C-library.
     */
    IpmiRaw raw;
    IpmiUpdateHelper ipmih(&raw);
    std::unique_ptr<DataInterface> handler;

    if (interface == "ipmibt")
    {
        handler = std::make_unique<BtDataHandler>(&ipmih);
    }

    if (handler == nullptr)
    {
        throw std::runtime_error("Unable to build interface handler.");
    }

    // UploadManager updater(&ipmih, handler.get());
    // return updater.UpdateBMC(imageStream, hashStream);
    return;
}
