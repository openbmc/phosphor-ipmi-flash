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
#include <fstream>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>

/* systemd service to kick start a service. */
static constexpr auto systemdService = "org.freedesktop.systemd1";
static constexpr auto systemdRoot = "/org/freedesktop/systemd1";
static constexpr auto systemdInterface = "org.freedesktop.systemd1.Manager";
static constexpr auto verifyTarget = "verify_image.service";

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
    closeFile(&hashFd);
}

void FlashUpdate::deleteEverything()
{
    /* Assumes you've called closeEverything() already */

    (void)std::remove(tmpPath.c_str());
    (void)std::remove(verifyPath.c_str());

    /* hashPath is optional. */
    if (!hashPath.empty())
    {
        (void)std::remove(hashPath.c_str());
    }
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

    /* Stop the systemd unit if it was running. */
    auto method = bus.new_method_call(systemdService, systemdRoot,
                                      systemdInterface, "StopUnit");
    method.append(verifyTarget);
    method.append("replace");
    bus.call_noreply(method);

    deleteEverything();
    return;
}

bool FlashUpdate::openEverything()
{
    flashFd = std::fopen(tmpPath.c_str(), "wb");
    if (flashFd == nullptr)
    {
        log<level::INFO>("Unable to open staging path",
                         entry("PATH=%s", tmpPath.c_str()));
        return false;
    }

    /* hash path is basically optional. */
    if (!hashPath.empty())
    {
        hashFd = std::fopen(hashPath.c_str(), "wb");
        if (hashFd == nullptr)
        {
            log<level::INFO>("Unable to open hash storage path",
                             entry("PATH=%s", hashPath.c_str()));
            closeFile(&flashFd);
            return false;
        }
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

bool FlashUpdate::writeBlock(std::FILE* fd, uint32_t offset,
                             const std::vector<uint8_t>& bytes)
{
    /* Seek into position, let's assume fseek won't call if offset matches
     * position.
     */
    if (std::fseek(fd, offset, SEEK_SET))
    {
        log<level::ERR>("Unable to seek into file to write bytes.");
        return false;
    }

    /* Write the bytes. */
    auto written = std::fwrite(bytes.data(), 1, bytes.size(), fd);

    if (written != bytes.size())
    {
        log<level::ERR>("Unable to write all the bytes requested.");
        return false;
    }

    (void)std::fflush(fd);
    return true;
}

bool FlashUpdate::flashData(uint32_t offset, const std::vector<uint8_t>& bytes)
{
    if (flashFd)
    {
        return writeBlock(flashFd, offset, bytes);
    }

    return false;
}

bool FlashUpdate::flashFinish()
{
    /* If it's open, close it. */
    if (flashFd)
    {
        closeFile(&flashFd);
        return true;
    }

    return false;
}

bool FlashUpdate::startHash(uint32_t length)
{
    if (!hashFd)
    {
        return false;
    }

    hashLength = length;
    return true;
}

bool FlashUpdate::hashData(uint32_t offset, const std::vector<uint8_t>& bytes)
{
    if (hashFd)
    {
        return writeBlock(hashFd, offset, bytes);
    }

    return false;
}

bool FlashUpdate::hashFinish()
{
    if (hashFd)
    {
        closeFile(&hashFd);
        return true;
    }

    return false;
}

bool FlashUpdate::startDataVerification()
{
    /* TODO: Look for injection point to test this. */
    auto method = bus.new_method_call(systemdService, systemdRoot,
                                      systemdInterface, "StartUnit");
    method.append(verifyTarget);
    method.append("replace");
    bus.call_noreply(method);

    /* TODO: implement. */
    return false;
}

bool FlashUpdate::abortUpdate()
{
    /* TODO: implement. */
    return false;
}

VerifyCheckResponse FlashUpdate::checkVerify()
{
    auto result = VerifyCheckResponse::other;
    std::ifstream ifs;
    ifs.open(verifyPath);
    if (ifs.good())
    {
        std::string status;
        /*
         * Check for the contents of the file, excepting:
         * running, success, or failed.
         */
        ifs >> status;
        if (status == "running")
        {
            result = VerifyCheckResponse::running;
        }
        else if (status == "success")
        {
            result = VerifyCheckResponse::success;
        }
        else if (status == "failed")
        {
            result = VerifyCheckResponse::failed;
        }
    }

    return result;
}
