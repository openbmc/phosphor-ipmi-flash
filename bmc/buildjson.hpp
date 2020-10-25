#pragma once
#include "general_systemd.hpp"
#include "image_handler.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ipmi_flash
{
std::unique_ptr<TriggerableActionInterface>
    buildFileSystemd(const nlohmann::json& data);

std::unique_ptr<TriggerableActionInterface>
    buildSystemd(const nlohmann::json& data);

template <typename T>
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
    std::unique_ptr<T> actions;
};

/* T is the action type */
template <typename T>
class handlersBuilderIfc
{
  public:
    handlersBuilderIfc() = default;
    ~handlersBuilderIfc() = default;
    handlersBuilderIfc(const handlersBuilderIfc&) = delete;
    handlersBuilderIfc& operator=(const handlersBuilderIfc&) = delete;
    handlersBuilderIfc(handlersBuilderIfc&&) = default;
    handlersBuilderIfc& operator=(handlersBuilderIfc&&) = default;
    /**
     * Given a folder of json configs, build the configurations.
     *
     * @param[in] directory - the directory to search (recurisvely).
     * @return list of HandlerConfig objects.
     */
    std::vector<HandlerConfig<T>>
        buildHandlerConfigs(const std::string& directory);
    /**
     * Given a list of handlers as json data, construct the appropriate
     * HandlerConfig objects.  This method is meant to be called per json
     * configuration file found.
     *
     * The list will only contain validly build HandlerConfig objects.  Any
     * invalid configuration is skipped. The hope is that the BMC firmware
     * update configuration will never be invalid, but if another aspect is
     * invalid, it can be fixed with a BMC firmware update once the bug is
     * identified.
     *
     * This code does not validate that the blob specified is unique, that
     * should be handled at a higher layer.
     *
     * @param[in] data - json data from a json file.
     * @return list of HandlerConfig objects.
     */
    virtual std::vector<HandlerConfig<T>>
        buildHandlerFromJson(const nlohmann::json& data) = 0;
};
} // namespace ipmi_flash
