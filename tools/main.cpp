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

#include "blob_handler.hpp"
#include "bt.hpp"
#include "ipmi_handler.hpp"
#include "lpc.hpp"
#include "updater.hpp"

/* Use CLI11 argument parser once in openbmc/meta-oe or whatever. */
#include <getopt.h>

#include <algorithm>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define IPMILPC "ipmilpc"
#define IPMIBT "ipmibt"

namespace
{
const std::vector<std::string> interfaceList = {IPMIBT, IPMILPC};
}

void usage(const char* program)
{
    std::ostringstream intfs;

    /* can use std::accumulate(), also probably loads of better ways
     */
    for (const auto& intf : interfaceList)
    {
        intfs << intf << ", ";
    }

    std::fprintf(stderr,
                 "Usage: %s -command <command> -interface <interface> -image "
                 "<image file> -sig <signature file>\n",
                 program);

    std::fprintf(stderr, "interfaces: %s", intfs.str().c_str());
}

bool checkCommand(const std::string& command)
{
    return (command == "update");
}

bool checkInterface(const std::string& interface)
{
    auto intf = std::find_if(
        interfaceList.begin(), interfaceList.end(),
        [interface](const auto& iter) { return (interface == iter); });
    return (intf != interfaceList.end());
}

int main(int argc, char* argv[])
{
    std::string command, interface, imagePath, signaturePath;

    while (1)
    {
        // clang-format off
        static struct option long_options[] = {
            {"command", required_argument, 0, 'c'},
            {"interface", required_argument, 0, 'i'},
            {"image", required_argument, 0, 'm'},
            {"sig", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };
        // clang-format on

        int option_index = 0;
        int c =
            getopt_long(argc, argv, "c:i:m:s:", long_options, &option_index);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'c':
                command = std::string{optarg};
                if (!checkCommand(command))
                {
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }

                break;
            case 'i':
                interface = std::string{optarg};
                if (!checkInterface(interface))
                {
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                imagePath = std::string{optarg};
                break;
            case 's':
                signaturePath = std::string{optarg};
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* They want to update the firmware. */
    if (command == "update")
    {
        if (interface.empty() || imagePath.empty() || signaturePath.empty())
        {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }

        IpmiHandler ipmi;
        BlobHandler blob(&ipmi);

        std::unique_ptr<DataInterface> handler;

        /* Input has already been validated in this case. */
        if (interface == IPMIBT)
        {
            handler = std::make_unique<BtDataHandler>(&blob);
        }
        else if (interface == IPMILPC)
        {
            handler = std::make_unique<LpcDataHandler>(&blob);
        }

        if (!handler)
        {
            /* TODO(venture): use a custom exception. */
            std::fprintf(stderr, "Interface %s is unavailable\n",
                         interface.c_str());
            exit(EXIT_FAILURE);
        }

        /* The parameters are all filled out. */
        return updaterMain(&blob, handler.get(), imagePath, signaturePath);
    }

    return 0;
}
