#include "hash_handler.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{

bool HashFileHandler::open(const std::string& path)
{
    this->path = path;

    if (hashFile.is_open())
    {
        /* This wasn't properly closed somehow.
         * TODO: Throw an error or just reset the state?
         */
        return false;
    }

    /* using ofstream no need to set out */
    hashFile.open(hashFilename, std::ios::binary);
    if (hashFile.bad())
    {
        /* TODO: Oh no! Care about this. */
        return false;
    }

    /* We were able to open the file for staging.
     * TODO: We'll need to do other stuff to eventually.
     */
    return true;
}

void HashFileHandler::close()
{
    if (hashFile.is_open())
    {
        hashFile.close();
    }
    return;
}

bool HashFileHandler::write(std::uint32_t offset,
                            const std::vector<std::uint8_t>& data)
{
    if (!hashFile.is_open())
    {
        return false;
    }

    /* We could track this, but if they write in a scattered method, this is
     * easier.
     */
    hashFile.seekp(offset, std::ios_base::beg);
    if (!hashFile.good())
    {
        /* the documentation wasn't super clear on fail vs bad in these cases,
         * so let's only be happy with goodness.
         */
        return false;
    }

    hashFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!hashFile.good())
    {
        return false;
    }

    return true;
}

} // namespace blobs
