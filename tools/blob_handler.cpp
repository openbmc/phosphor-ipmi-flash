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

#include "blob_handler.hpp"

int BlobHandler::getBlobCount()
{
    return 0;
}

std::string BlobHandler::enumerateBlob(int index)
{
    return "";
}

std::vector<std::string> BlobHandler::getBlobList()
{
    std::vector<std::string> list;
    int blobCount = getBlobCount();

    for (int i = 0; i < blobCount; i++)
    {
        auto name = enumerateBlob(i);
        /* Currently ignore failures. */
        if (!name.empty())
        {
            list.push_back(name);
        }
    }

    return list;
}
