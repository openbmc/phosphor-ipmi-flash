#include "bt.hpp"

#include <cstdint>
#include <ipmiblob/blob_errors.hpp>
#include <vector>

namespace host_tool
{

bool BtDataHandler::sendContents(const std::string& input,
                                 std::uint16_t session)
{
    int inputFd = sys->open(input.c_str(), 0);
    if (inputFd < 0)
    {
        return false;
    }

    static constexpr int btBufferLen = 50;
    std::uint8_t readBuffer[btBufferLen];
    int bytesRead;
    std::uint32_t offset = 0;

    try
    {
        do
        {
            bytesRead = sys->read(inputFd, readBuffer, sizeof(readBuffer));
            if (bytesRead > 0)
            {
                /* minorly awkward repackaging. */
                std::vector<std::uint8_t> buffer(&readBuffer[0],
                                                 &readBuffer[bytesRead]);
                blob->writeBytes(session, offset, buffer);
                offset += bytesRead;
            }
        } while (bytesRead > 0);
    }
    catch (const ipmiblob::BlobException& b)
    {
        sys->close(inputFd);
        return false;
    }

    sys->close(inputFd);
    return true;
}

} // namespace host_tool
