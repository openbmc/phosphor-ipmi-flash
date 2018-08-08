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

#include <experimental/filesystem>
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

std::unique_ptr<UploadManager> UploadManager::BuildUploadMgr(
    const std::string& image, const std::string& hash,
    UpdateHelperInterface* helper, DataInterface* dintf)
{
    std::ifstream imageStream, hashStream;

    imageStream.open(image);
    hashStream.open(hash);
    if (imageStream.bad() || hashStream.bad())
    {
        return nullptr;
    }

    int32_t imageSize = std::experimental::filesystem::file_size(image);
    int32_t hashSize = std::experimental::filesystem::file_size(hash);

    return std::make_unique<UploadManager>(std::move(imageStream),
                                           std::move(hashStream), imageSize,
                                           hashSize, helper, dintf);
}

void UploadManager::UpdateBMC()
{
    /* Let's build the raw command input.
     *
     * The sequence is:
     * FLASH_START_TRANSFER,
     * FLASH_DATA_BLOCK x times. (or FLASH_EXTERNAL_DATA_BLOCK)
     * FLASH_DATA_FINISH
     * FLASH_START_HASH
     * FLASH_HASH_DATA x times. (or FLASH_EXTERNAL_HASH_BLOCK)
     * FLASH_HASH_FINISH
     * FLASH_DATA_VERIFY
     * FLASH_VERIFY_CHECK x times.
     */

    /* TODO: implement this. */

    /* UploadImage() */
    /* UploadHash() */

    /*
     * FLASH_DATA_VERIFY - The verify command will trigger verification of the
     * image against the signature file sent down.
     */
    //  ret = helper_->SendEmptyCommand(FLASH_DATA_VERIFY, nullptr);
    //  if (!ret.ok()) return ret;

    return;
}

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

    auto updater =
        UploadManager::BuildUploadMgr(image, signature, &ipmih, handler.get());

    if (updater == nullptr)
    {
        throw std::runtime_error("Unable to build update manager.");
    }

    updater->UpdateBMC();

    return;
}
