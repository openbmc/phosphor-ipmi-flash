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

#include <cstdint>
#include <ios>
#include <memory>
#include <string>
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
    if (file.is_open())
    {
        file.close();
    }
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
    std::vector<uint8_t> ret(size);
    file.seekg(offset);
    size_t done = 0;
    while (ssize_t amt = file.readsome(ret.data() + done, size - done); amt > 0)
    {
        done += amt;
    }
    return file.good() ? std::move(ret) : std::nullopt;
}

int FileHandler::getSize()
{
    file.seekg(0, file.end);
    return file.tellg();
}

} // namespace ipmi_flash
