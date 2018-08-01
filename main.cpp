/*
 * Copyright 2017 Google Inc.
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

#include <memory>

#include "host-ipmid/ipmid-api.h"
#include "host-ipmid/oemrouter.hpp"

#include "flash-ipmi.hpp"
#include "ipmi.hpp"

/* TODO: Once OEM IPMI number placement is settled, point to that. */
namespace oem
{
namespace google
{
constexpr int number = 11129;
constexpr int flashOverBTCmd = 127;
} // namespace google
} // namespace oem

/* We have one instance of the FlashUpdate object tracking commands. */
std::unique_ptr<FlashUpdate> flashUpdateSingleton;

static ipmi_ret_t flashControl(ipmi_cmd_t cmd, const uint8_t* reqBuf,
                               uint8_t* replyCmdBuf, size_t* dataLen)
{
    /* Verify it's at least as long as the shortest message. */
    if ((*dataLen) < 1)
    {
        return IPMI_CC_INVALID;
    }

    uint8_t subCmd = reqBuf[0];

    /* TODO: This could be cleaner to just have a function pointer table, may
     * transition in later patchset.
     */
    switch (subCmd)
    {
        case FlashSubCmds::flashStartTransfer:
            return startTransfer(flashUpdateSingleton.get(), reqBuf,
                                 replyCmdBuf, dataLen);
        case FlashSubCmds::flashDataBlock:
            return dataBlock(flashUpdateSingleton.get(), reqBuf,
                             replyCmdBuf, dataLen);
    }

    return IPMI_CC_INVALID;
}

static ipmi_ret_t ipmiFlashControl(ipmi_netfn_t netFn, ipmi_cmd_t cmd,
                                   ipmi_request_t request,
                                   ipmi_response_t response,
                                   ipmi_data_len_t dataLen,
                                   ipmi_context_t context)
{
    /* request_t, response_t are void*. ipmi_data_len_t is a size_t* */
    auto reqBuf = static_cast<const uint8_t*>(request);
    auto replyCmdBuf = static_cast<uint8_t*>(response);

    return flashControl(cmd, reqBuf, replyCmdBuf, dataLen);
}

void setupGlobalOemFlashControl() __attribute__((constructor));

void setupGlobalOemFlashControl()
{
    flashUpdateSingleton = std::make_unique<FlashUpdate>();

#ifdef ENABLE_GOOGLE
    oem::Router* router = oem::mutableRouter();

    fprintf(stderr, "Registering OEM:[%#08X], Cmd:[%#04X] for Flash Update\n",
            oem::google::number, oem::google::flashOverBTCmd);

    router->registerHandler(oem::google::number, oem::google::flashOverBTCmd,
                            flashControl);
#endif

    ipmi_register_callback(NETFUN_FIRMWARE, oem::google::flashOverBTCmd, NULL,
                           ipmiFlashControl, PRIVILEGE_USER);
}
