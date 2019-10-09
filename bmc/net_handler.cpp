#include "net_handler.hpp"

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>

namespace ipmi_flash
{

bool NetDataHandler::open()
{
    listenFd.reset(::socket(AF_INET6, SOCK_STREAM, 0));
    if (*listenFd < 0)
    {
        std::perror("Failed to create socket");
        (void)listenFd.release();
        return false;
    }

    struct sockaddr_in6 listenAddr;
    listenAddr.sin6_family = AF_INET6;
    listenAddr.sin6_port = htons(listenPort);
    listenAddr.sin6_flowinfo = 0;
    listenAddr.sin6_addr = in6addr_any;
    listenAddr.sin6_scope_id = 0;

    if (::bind(*listenFd, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) <
        0)
    {
        std::perror("Failed to bind");
        return false;
    }

    if (::listen(*listenFd, 1) < 0)
    {
        std::perror("Failed to listen");
        return false;
    }
    return true;
}

bool NetDataHandler::close()
{
    connFd.reset();
    listenFd.reset();

    return true;
}

std::vector<std::uint8_t> NetDataHandler::copyFrom(std::uint32_t length)
{
    if (!connFd)
    {
        connFd.reset(::accept(*listenFd, nullptr, nullptr));
        if (*connFd < 0)
        {
            std::perror("Failed to accept connection");
            (void)connFd.release();
            return std::vector<uint8_t>();
        }
    }

    std::vector<std::uint8_t> data(length);

    std::uint32_t bytesRead = 0;
    int ret;
    do
    {
        ret = read(*connFd, data.data() + bytesRead, length - bytesRead);
        if (ret < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            std::perror("Failed to read from socket");
            break;
        }

        bytesRead += ret;
    } while (ret > 0);

    if (bytesRead != length)
        fprintf(stderr,
                "Couldn't read full expected amount. Wanted %u but got %u\n",
                length, bytesRead);

    return data;
}

bool NetDataHandler::writeMeta(const std::vector<std::uint8_t>& configuration)
{
    // TODO: have the host tool send the expected IP address that it will
    // connect from
    return true;
}

std::vector<std::uint8_t> NetDataHandler::readMeta()
{
    return std::vector<std::uint8_t>();
}

} // namespace ipmi_flash
