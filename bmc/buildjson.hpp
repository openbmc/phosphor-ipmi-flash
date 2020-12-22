#pragma once

#include "fs.hpp"
#include "general_systemd.hpp"
#include "image_handler.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ipmi_flash
{
/**
 * build a systemd file triggerable action from json data
 */
std::unique_ptr<TriggerableActionInterface>
    buildFileSystemd(const nlohmann::json& data);

/**
 * build a systemd triggerable action from json data
 */
std::unique_ptr<TriggerableActionInterface>
    buildSystemd(const nlohmann::json& data);

/**
 * HandlerConfig associates a blobID with an ImageHandler and a set of
 * supported actions of type T.
 */
template <typename T>
struct HandlerConfig
{
    /* A string in the form: /flash/{unique}, s.t. unique is something like,
     * flash, ubitar, statictar, or bios
     */
    std::string blobId;

    /* This owns a handler interface, this is typically going to be a file
     * writer object.
     */
    std::unique_ptr<ImageHandlerInterface> handler;

    /*  specifies actions to be taken in response to certain operations on a
     *  blob.
     *  Usually required but there are exceptions;  the hashBlobId doesn't have
     * an action pack.
     */
    std::unique_ptr<T> actions;
};

/* HandlersBuilderIfc is a helper class that builds Handlers from the json files
 * found within a specified directory.
 * The child class that inherits from HandlersBuilderIfc should implement
 * buildHandlersConfigs to perform json validation and parsing.
 *
 */
template <typename T>
struct HandlersBuilderIfc
{
    virtual ~HandlersBuilderIfc() = default;

    /**
     * Given a folder of json configs, build the configurations.
     *
     * @param[in] directory - the directory to search (recurisvely).
     * @return list of HandlerConfig objects.
     */
    std::vector<HandlerConfig<T>>
        buildHandlerConfigs(const std::string& directory)
    {

        std::vector<HandlerConfig<T>> output;

        std::vector<std::string> jsonPaths = GetJsonList(directory);
        for (const auto& path : jsonPaths)
        {
            std::ifstream jsonFile(path);
            if (!jsonFile.is_open())
            {
                std::fprintf(stderr, "Unable to open json file: %s\n",
                             path.c_str());
                continue;
            }

            auto data = nlohmann::json::parse(jsonFile, nullptr, false);
            if (data.is_discarded())
            {
                std::fprintf(stderr, "Parsing json failed: %s\n", path.c_str());
                continue;
            }

            std::vector<HandlerConfig<T>> configs = buildHandlerFromJson(data);
            std::move(configs.begin(), configs.end(),
                      std::back_inserter(output));
        }
        return output;
    };

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
