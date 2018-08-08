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

#include <getopt.h>

#include <cstdio>
#include <stdexcept>
#include <string>

#include "updater.hpp"

static void usage(const char* name)
{
    std::fprintf(
        stderr,
        "Usage: %s -c <COMMAND> -i <INTERFACE> -m <IMAGE> -s <SIGNATURE>\n"
        "  -c, --command <COMMAND>     the command to run 'update', 'reboot', "
        "'ping'\n"
        "  -i, --interface <INTERFACE> the interface to use, 'ipmibt'\n"
        "  -m, --imaage <IMAGE>        the image file\n"
        "  -s, --signature <SIGNATURE> the signature file\n",
        name);
}

int main(int argc, char* argv[])
{
    int opt;

    static const struct option long_options[] = {
        {"command", required_argument, NULL, 'c'},
        {"interface", required_argument, NULL, 'i'},
        {"image", required_argument, NULL, 'm'},
        {"signature", required_argument, NULL, 's'},
        {0, 0, 0, 0}};
    const char* pm = "c:i:m:s:";

    std::string command, interface, image, signature;

    while ((opt = getopt_long(argc, argv, pm, long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 0:
                break;
            case 'c':
                command = optarg;
                break;
            case 'i':
                interface = optarg;
                break;
            case 'm':
                image = optarg;
                break;
            case 's':
                signature = optarg;
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (command == "update")
    {
        if (interface.empty() || image.empty() || signature.empty())
        {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        try
        {
            UpdaterMain(interface, image, signature);
            return 0;
        }
        catch (const std::runtime_error& e)
        {
            fprintf(stderr, "runtime error: %s\n", e.what());
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
