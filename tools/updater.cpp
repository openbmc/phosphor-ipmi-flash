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

#include "firmware_handler.hpp"
#include "handler.hpp"
#include "status.hpp"
#include "tool_errors.hpp"
#include "util.hpp"

#include <algorithm>
#include <blobs-ipmid/blobs.hpp>
#include <cstring>
#include <ipmiblob/blob_errors.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace host_tool
{

void updaterMain(UpdateHandlerInterface* updater, const std::string& imagePath,
                 const std::string& signaturePath)
{
    /* TODO(venture): Add optional parameter to specify the flash type, default
     * to legacy for now.
     */
    bool goalSupported =
        updater->checkAvailable(ipmi_flash::staticLayoutBlobId);
    if (!goalSupported)
    {
        throw ToolException("Goal firmware or interface not supported");
    }

    /* Yay, our data handler is supported. */
    try
    {

        /* Send over the firmware image. */
        std::fprintf(stderr, "Sending over the firmware image.\n");
        updater->sendFile(ipmi_flash::staticLayoutBlobId, imagePath);

        /* Send over the hash contents. */
        std::fprintf(stderr, "Sending over the hash file.\n");
        updater->sendFile(ipmi_flash::hashBlobId, signaturePath);

        /* Trigger the verification by opening and committing the verify file.
         */
        std::fprintf(stderr, "Opening the verification file\n");
        if (updater->verifyFile(ipmi_flash::verifyBlobId))
        {
            std::fprintf(stderr, "succeeded\n");
        }
        else
        {
            std::fprintf(stderr, "failed\n");
            throw ToolException("Verification failed");
        }

        /* Trigger the update by opening and committing the update file. */
        std::fprintf(stderr, "Opening the update file\n");
        if (updater->verifyFile(ipmi_flash::updateBlobId))
        {
            std::fprintf(stderr, "succeeded\n");
        }
        else
        {
            /* Depending on the update mechanism used, this may be
             * uninteresting. For instance, for the static layout, we use the
             * reboot update mechanism.  Which doesn't always lead to a
             * successful return before the BMC starts shutting down services.
             */
            std::fprintf(stderr, "failed\n");
            throw ToolException("Update failed");
        }
    }
    catch (...)
    {
        updater->cleanArtifacts();
        throw;
    }
}

} // namespace host_tool
