#pragma once
#include "buildjson.hpp"
#include "status.hpp"

#include <blobs-ipmid/blobs.hpp>

#include <memory>
namespace ipmi_flash
{
struct VersionActionPack
{
  public:
    VersionActionPack(std::unique_ptr<TriggerableActionInterface> openAction) :
        onOpen(std::move(openAction)){};
    VersionActionPack() = default;
    /** Only file operation action supported currently */
    std::unique_ptr<TriggerableActionInterface> onOpen;
};
} // namespace ipmi_flash
