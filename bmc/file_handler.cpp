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

#include "file_handler.hpp"

#include <ios>
#include <optional>
#include <utility>
#include <vector>

namespace ipmi_flash
{

bool FileHandler::open(const std::string& path, std::ios_base::openmode mode)
{
    /* force binary mode */
    mode |= std::ios::binary;
    this->path = path;

    if (file.is_open())
    {
        return true;
    }
    file.open(filename, mode);
    return file.good();
}

void FileHandler::close()
{
    file.close();
}

bool FileHandler::write(std::uint32_t offset,
                        const std::vector<std::uint8_t>& data)
{
    file.seekp(offset);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

std::optional<std::vector<uint8_t>> FileHandler::read(std::uint32_t offset,
                                                      std::uint32_t size)
{
    std::vector<uint8_t> ret(std::min(getSize() - offset, size));
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(ret.data()), ret.size());
    if (!file.good())
    {
        return std::nullopt;
    }
    return std::move(ret);
}

int FileHandler::getSize()
{
    file.seekg(0, file.end);
    return file.tellg();
}

} // namespace ipmi_flash
