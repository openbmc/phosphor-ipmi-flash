#pragma once
#include "image_handler.hpp"

#include <memory>
#include <string>

namespace ipmi_flash
{

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

} // namespace ipmi_flash
