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
    if (!stagedOutput.is_open())
    {
        return false;
    }

    /* We could track this, but if they write in a scattered method, this is
     * easier.
     */
    stagedOutput.seekp(offset, std::ios_base::beg);
    if (!stagedOutput.good())
    {
        /* the documentation wasn't super clear on fail vs bad in these cases,
         * so let's only be happy with goodness.
         */
        return false;
    }

    stagedOutput.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!stagedOutput.good())
    {
        return false;
    }

    return true;
}

} // namespace blobs
