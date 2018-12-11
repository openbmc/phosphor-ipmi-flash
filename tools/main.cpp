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
#include "updater.hpp"

/* Use CLI11 argument parser once in openbmc/meta-oe or whatever. */
#include <getopt.h>

#include <cstdio>
#include <memory>
#include <string>

void usage(const char* program)
{
    std::fprintf(stderr,
                 "Usage: %s -command <command> -interface <interface> -image "
                 "<image file> -sig <signature file>",
                 program);
}

bool checkCommand(const std::string& command)
{
    return (command == "update");
}

bool checkInterface(const std::string& interface)
{
    return (interface == "ipmibt" || interface == "ipmilpc");
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

    /* They want to updater the firmware. */
    if (command == "update")
    {
        if (interface.empty() || imagePath.empty() || signaturePath.empty())
        {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }

        auto blob = std::make_unique<BlobHandler>();

        /* The parameters are all filled out. */
        return updaterMain(blob.get(), interface, imagePath, signaturePath);
    }

    return 0;
}
