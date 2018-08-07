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

#include "flash-ipmi.hpp"

#include <cstdio>
#include <phosphor-logging/log.hpp>

using namespace phosphor::logging;

namespace
{

/**
 * Close a file pointer and set to null.
 *
 * @param[in] fd a pointer to your file handle.
 */
void closeFile(std::FILE** fd)
{
    if (!fd)
    {
        return;
    }

    if (*fd)
    {
        std::fclose(*fd);
        (*fd) = nullptr;
    }
}
}

void FlashUpdate::closeEverything()
{
    closeFile(&flashFd);
}

FlashUpdate::~FlashUpdate()
{
    /* Close without deleting.  This object can only be destroyed if the ipmi
     * daemon unloads it, by closing down.  In this event, we want the verified
     * file to live on.
     */
    closeEverything();
}

void FlashUpdate::abortEverything()
{
    closeEverything();

    /* TODO: And now delete everything */
    return;
}

bool FlashUpdate::openEverything()
{
    flashFd = std::fopen(tmpPath.c_str(), "w");
    if (flashFd == nullptr)
    {
        log<level::INFO>("Unable to open staging path",
                         entry("PATH=%s", tmpPath.c_str()));
        return false;
    }

    return true;
}

/* Prepare to receive a BMC image and then a signature. */
bool FlashUpdate::start(uint32_t length)
{
    /* Close out and delete everything. */
    abortEverything();

    /* TODO: Validate request->length */
    flashLength = length;

    /* Start over! */
    return openEverything();
}

bool FlashUpdate::flashData(uint32_t offset, const std::vector<uint8_t>& bytes)
{
    /* TODO: implement. */
    return false;
}

bool FlashUpdate::flashFinish()
{
    /* TODO: implement. */
    return false;
}

bool FlashUpdate::startHash(uint32_t length)
{
    /* TODO: implement. */
    return false;
}

bool FlashUpdate::hashData(uint32_t offset, const std::vector<uint8_t>& bytes)
{
    /* TODO: implement. */
    return false;
}

bool FlashUpdate::hashFinish()
{
    /* TODO: implement. */
    return false;
}

bool FlashUpdate::startDataVerification()
{
    /* TODO: implement. */
    return false;
}

bool FlashUpdate::abortUpdate()
{
    /* TODO: implement. */
    return false;
}
