// Copyright 2021 -l
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "handler_config.hpp"
#include "image_mock.hpp"
#include "log_handler.hpp"
#include "triggerable_mock.hpp"

#include <memory>
#include <string>
#include <utility>

namespace ipmi_flash
{

auto createMockLogConfig(const std::string& id, ImageHandlerMock** im = nullptr,
                         TriggerMock** tm = nullptr)
{
    HandlerConfig<LogBlobHandler::ActionPack> ret;
    ret.blobId = id;
    auto handler = std::make_unique<testing::StrictMock<ImageHandlerMock>>();
    if (im != nullptr)
    {
        *im = handler.get();
    }
    ret.handler = std::move(handler);
    ret.actions = std::make_unique<LogBlobHandler::ActionPack>();
    auto trigger = std::make_unique<testing::StrictMock<TriggerMock>>();
    if (tm != nullptr)
    {
        *tm = trigger.get();
    }
    ret.actions->onOpen = std::move(trigger);
    return ret;
}

template <typename C, typename Im = std::map<std::string, ImageHandlerMock*>,
          typename Tm = std::map<std::string, TriggerMock*>>
auto createMockLogConfigs(const C& ids, Im* im = nullptr, Tm* tm = nullptr)
{
    std::vector<HandlerConfig<LogBlobHandler::ActionPack>> ret;
    for (const auto& id : ids)
    {
        ret.push_back(
            createMockLogConfig(id, im == nullptr ? nullptr : &(*im)[id],
                                tm == nullptr ? nullptr : &(*tm)[id]));
    }
    return ret;
}
} // namespace ipmi_flash
