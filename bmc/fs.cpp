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

#include "fs.hpp"

#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

namespace ipmi_flash
{
namespace fs = std::filesystem;

std::vector<std::string> GetJsonList(const std::string& directory)
{
    std::vector<std::string> output;

    try
    {
        for (const auto& p : fs::recursive_directory_iterator(directory))
        {
            auto path = p.path();
            if (path.extension().string() == ".json")
            {
                output.push_back(path.string());
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        // Ignore missing directories and just return an empty list
        if (e.code() == std::error_code(ENOENT, std::generic_category()))
        {
            return output;
        }
        throw;
    }

    return output;
}

} // namespace ipmi_flash
