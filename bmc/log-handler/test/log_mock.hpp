#include <memory>
#include <string>
#include <utility>

#include "handler_config.hpp"
#include "image_mock.hpp"
#include "log_handler.hpp"
#include "triggerable_mock.hpp"

namespace ipmi_flash {

auto createMockLogConfig(const std::string& id, ImageHandlerMock** im = nullptr,
                         TriggerMock** tm = nullptr) {
  HandlerConfig<LogBlobHandler::ActionPack> ret;
  ret.blobId = id;
  auto handler = std::make_unique<testing::StrictMock<ImageHandlerMock>>();
  if (im != nullptr) {
    *im = handler.get();
  }
  ret.handler = std::move(handler);
  ret.actions = std::make_unique<LogBlobHandler::ActionPack>();
  auto trigger = std::make_unique<testing::StrictMock<TriggerMock>>();
  if (tm != nullptr) {
    *tm = trigger.get();
  }
  ret.actions->onOpen = std::move(trigger);
  return ret;
}

template <typename C, typename Im = std::map<std::string, ImageHandlerMock*>,
          typename Tm = std::map<std::string, TriggerMock*>>
auto createMockLogConfigs(const C& ids, Im* im = nullptr, Tm* tm = nullptr) {
  std::vector<HandlerConfig<LogBlobHandler::ActionPack>> ret;
  for (const auto& id : ids) {
    ret.push_back(createMockLogConfig(id, im == nullptr ? nullptr : &(*im)[id],
                                      tm == nullptr ? nullptr : &(*tm)[id]));
  }
  return ret;
}
}  // namespace ipmi_flash
