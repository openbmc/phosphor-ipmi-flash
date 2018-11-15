#include "static_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

bool StaticLayoutHandler::open(const std::string& path)
{
    this->path = path;

    if (stagedOutput.is_open())
    {
        /* This wasn't properly closed somehow.
         * TODO: Throw an error or just reset the state?
         */
        return false;
    }

    /* using ofstream no need to set out */
    stagedOutput.open(stagedFilename, std::ios::binary);
    if (stagedOutput.bad())
    {
        /* TODO: Oh no! Care about this. */
        return false;
    }

    /* We were able to open the file for staging.
     * TODO: We'll need to do other stuff to eventually.
     */
    return true;
}

void StaticLayoutHandler::close()
{
    if (stagedOutput.is_open())
    {
        stagedOutput.close();
    }
    return;
}

bool StaticLayoutHandler::write(std::uint32_t offset,
                                const std::vector<std::uint8_t>& data)
{
    return false;
}

} // namespace blobs
