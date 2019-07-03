#pragma once

#include "firmware_handler.hpp"
#include "image_handler.hpp"

#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace ipmi_flash
{

class HandlerConfig
{
  public:
    HandlerConfig() = default;
    ~HandlerConfig() = default;
    HandlerConfig(const HandlerConfig&) = delete;
    HandlerConfig& operator=(const HandlerConfig&) = delete;
    HandlerConfig(HandlerConfig&&) = default;
    HandlerConfig& operator=(HandlerConfig&&) = default;

    /* A string in the form: /flash/{unique}, s.t. unique is something like,
     * flash, ubitar, statictar, or bios
     */
    std::string blobId;

    /* This owns a handler interface, this is typically going to be a file
     * writer object.
     */
    std::unique_ptr<ImageHandlerInterface> handler;

    /* Only the hashBlobId doesn't have an action pack, otherwise it's required.
     */
    std::unique_ptr<ActionPack> actions;
};

HandlerConfig buildHandlerFromJson(const nlohmann::json& data);

} // namespace ipmi_flash
