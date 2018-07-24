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

#include "host-ipmid/ipmid-api.h"
#include "host-ipmid/oemgoog.hpp"
#include "host-ipmid/oemrouter.hpp"

static ipmi_ret_t
flashControl(
    ipmi_cmd_t     cmd,
    const uint8_t* reqBuf,
    uint8_t*       replyCmdBuf,
    size_t*        dataLen)
{
    /* Verify it's at least as long as the shortest message. */
    if ((*dataLen) < 1)
    {
        return IPMI_CC_INVALID;
    }

    return IPMI_CC_INVALID;
}

void setupGlobalOemFlashControl() __attribute__((constructor));

void setupGlobalOemFlashControl()
{
    oem::Router* router = oem::mutableRouter();

    fprintf(stderr,
            "Registering OEM:[%#08X], Cmd:[%#04X] for Flash Update\n",
            oem::google::googOemNumber,
            oem::google::flashOverBTCmd);

    router->registerHandler(
        oem::google::googOemNumber,
        oem::google::flashOverBTCmd,
        flashControl);
}
