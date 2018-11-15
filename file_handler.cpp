#include "file_handler.hpp"

#include <cstdint>
#include <experimental/filesystem>
#include <memory>
#include <string>
#include <vector>

namespace blobs
{
namespace fs = std::experimental::filesystem;

bool FileHandler::open(const std::string& path)
{
    this->path = path;

    if (file.is_open())
    {
        /* This wasn't properly closed somehow.
         * TODO: Throw an error or just reset the state?
         */
        return false;
    }

    /* using ofstream no need to set out */
    file.open(filename, std::ios::binary);
    if (file.bad())
    {
        /* TODO: Oh no! Care about this. */
        return false;
    }

    /* We were able to open the file for staging.
     * TODO: We'll need to do other stuff to eventually.
     */
    return true;
}

void FileHandler::close()
{
    if (file.is_open())
    {
        file.close();
    }
    return;
}

bool FileHandler::write(std::uint32_t offset,
                        const std::vector<std::uint8_t>& data)
{
    if (!file.is_open())
    {
        return false;
    }

    /* We could track this, but if they write in a scattered method, this is
     * easier.
     */
    file.seekp(offset, std::ios_base::beg);
    if (!file.good())
    {
        /* the documentation wasn't super clear on fail vs bad in these cases,
         * so let's only be happy with goodness.
         */
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file.good())
    {
        return false;
    }

    return true;
}

int FileHandler::getSize()
{
    try
    {
        return static_cast<int>(fs::file_size(filename));
    }
    catch (const fs::filesystem_error& e)
    {
    }

    return 0;
}

} // namespace blobs
