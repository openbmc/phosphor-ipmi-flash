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

#include "raw.hpp"

#include <stdexcept>

struct IpmiResponse IpmiRaw::Raw(const std::vector<uint8_t>& buffer)
{
    struct IpmiResponse response;
    int rc = ipmiSendCommand(buffer.data(), buffer.size(), &response);
    if (rc)
    {
        throw std::runtime_error("Failure sending IPMI packet.");
    }

    return response;
}

struct IpmiResponse IpmiRaw::RawWithTries(const std::vector<uint8_t>& buffer,
                                          int tries)
{
    int count = 0;
    struct IpmiResponse response;

    /* If tries is 0, it'll run once. */
    do
    {
        try
        {
            response = Raw(buffer);
        }
        catch (const std::runtime_error& e)
        {
            continue;
        }

        count++;
        if (count >= tries)
        {
            throw std::runtime_error("Failure sending IPMI packet.");
        }
    } while (count < tries);

    return response;
}
