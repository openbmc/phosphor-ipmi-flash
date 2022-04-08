/*
 * Copyright 2019 Google Inc.
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

#include "net.hpp"

#include "data.hpp"
#include "flags.hpp"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <ipmiblob/blob_errors.hpp>
#include <stdplus/handle/managed.hpp>
#include <stdplus/raw.hpp>

#include <cstdint>
#include <cstring>
#include <optional>

namespace
{

void closefd(int&& fd, const internal::Sys*& sys)
{
    sys->close(fd);
}
using Fd = stdplus::Managed<int, const internal::Sys*>::Handle<closefd>;

} // namespace

namespace host_tool
{

bool NetDataHandler::sendContents(const std::string& input,
                                  std::uint16_t session)
{
    constexpr size_t blockSize = 64 * 1024;
    Fd inputFd(std::nullopt, sys);

    inputFd.reset(sys->open(input.c_str(), O_RDONLY));
    if (*inputFd < 0)
    {
        (void)inputFd.release();
        std::fprintf(stderr, "Unable to open file: '%s'\n", input.c_str());
        return false;
    }

    std::int64_t fileSize = sys->getSize(input.c_str());
    Fd connFd(std::nullopt, sys);

    {
        struct addrinfo hints;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_flags = AI_NUMERICHOST;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        struct addrinfo *addrs, *addr;
        int ret = sys->getaddrinfo(host.c_str(), port.c_str(), &hints, &addrs);
        if (ret < 0)
        {
            std::fprintf(stderr, "Couldn't parse address %s with port %s: %s\n",
                         host.c_str(), port.c_str(), gai_strerror(ret));
            return false;
        }

        for (addr = addrs; addr != nullptr; addr = addr->ai_next)
        {
            connFd.reset(sys->socket(addr->ai_family, addr->ai_socktype,
                                     addr->ai_protocol));
            if (*connFd == -1)
                continue;

            if (sys->connect(*connFd, addr->ai_addr, addr->ai_addrlen) != -1)
                break;
        }

        // TODO: use stdplus Managed for the addrinfo structs
        sys->freeaddrinfo(addrs);

        if (addr == nullptr)
        {
            std::fprintf(stderr, "Failed to connect\n");
            return false;
        }
    }

    try
    {
        int bytesSent = 0;
        off_t offset = 0;

        progress->start(fileSize);
        auto confirmSend =
            [&]() {
                if (bytesSent == 0)
                {
                    return;
                }
                struct ipmi_flash::ExtChunkHdr chunk;
                chunk.length = bytesSent;
                /* This doesn't return anything on success. */
                blob->writeBytes(session, offset - bytesSent,
                                 stdplus::raw::asSpan<uint8_t>(chunk));
                progress->updateProgress(bytesSent);
            }

        do
        {
            bytesSent = sys->sendfile(*connFd, *inputFd, &offset, blockSize);
            if (bytesSent < 0)
            {
                if (errno == EINVAL)
                {
                    std::array<uint8_t, 4096> buf;
                    size_t left = 0;
                    do
                    {
                        bytesSent = sys->read(*inputFd, buf.data() + left,
                                              buf.size() - left);
                        if (bytesSent < 0)
                        {
                            std::fprintf(stderr,
                                         "Failed read data for BMC: %s\n",
                                         strerror(errno));
                            progress->abort();
                            return false;
                        }
                        left += bytesSent;
                        bytesSent = sys->write(*connFd, buf.data(), left, 0);
                        if (bytesSent < 0)
                        {
                            std::fprintf(stderr,
                                         "Failed to send data to BMC: %s\n",
                                         strerror(errno));
                            progress->abort();
                            return false;
                        }
                        std::memmove(buf.data(), buf.data() + bytesSent,
                                     left - bytesSent);
                        left -= bytesSent;
                        confirmSend();
                    } while (bytesSent > 0);
                    break;
                }
                std::fprintf(stderr, "Failed to send data to BMC: %s\n",
                             strerror(errno));
                progress->abort();
                return false;
            }
            confirmSend();
        }
        while (bytesSent > 0)
    }
    catch (const ipmiblob::BlobException& b)
    {
        progress->abort();
        return false;
    }

    progress->finish();
    return true;
}

} // namespace host_tool
